#include "redis_api.h"

#include <ctime>

redis_api::redis_api(std::string file):    
    log_name(file) {
    std::ifstream get_data(file.c_str());
    std::string time_log_name = log_name + "_time";
    std::fstream get_time_log(time_log_name);
    read_map_from(data, get_data);
    read_time_log(key_time, get_time_log);
    get_data.close();
    get_time_log.close();

    log.open(file.c_str(), std::ios::out | std::ios::in | std::ios::binary);
//    change_log.open("change_log", std::ios::in | std::ios::out | std::ios::binary);
    change_time_log.open("change_time_log", std::ios::in | std::ios::out);

    apply_change_to_log();
}

redis_api::redis_api() {

}


redis_api::~redis_api() {
    log.close();
    change_log.close();
}

std::string redis_api::set(std::string key, std::string value) {
    data[key] = value;
    change_map.insert({key, value});
    //    upd_change_log();

    return "OK";
}

std::string redis_api::get(std::string key) {
    std::string ans = "$-1\r\n";
    auto it = data.find(key);
    if (it != data.end()) {
        if (valid_key(key))
            ans = data[key];
        else
            delete_key(key);
    }

    return ans;
}

void redis_api::upd_change_log() {
    if (change_map.empty())
        return;

    change_log.open("change_log", std::ios::out | std::ios::app | std::ios::binary);
    write_map_to(change_map, change_log);
    change_log.close();

    change_time_log.open("change_time_log", std::ios::out | std::ios::app | std::ios::binary);
    write_time_log(change_time, change_time_log);
    change_time_log.close();

    change_map.clear();
    change_time_log.clear();
}

void redis_api::apply_change_to_log() {
    change_log.open("change_log", std::ios::in | std::ios::binary);
    read_map_from(data, change_log);
    read_time_log(key_time, change_time_log);
    apply_deleted();

    std::ofstream tmp_log("tmp_log");
    write_map_to(data, tmp_log);
    std::string comand = "cp tmp_log " + log_name;
    system(comand.c_str());
    comand = "rm tmp_log";
    system(comand.c_str());

    std::ofstream tmp_time_log("tmp_time_log");
    write_time_log(key_time, tmp_time_log);
    comand = "cp tmp_time_log " + log_name + "_time";
    system(comand.c_str());
    comand = "rm tmp_time_log";
    system(comand.c_str());


    change_log.close();
    change_time_log.close();

    change_log.open("change_log", std::ios::in | std::ios::out | std::ios::trunc);
    change_time_log.open("change_time_log", std::ios::in | std::ios::out | std::ios::trunc);

}

void redis_api::read_map_from(std::unordered_map<std::string, std::string>& map, std::istream &in) {
    std::string key, val;
    while (read_string(in, key)) {
        if (!read_string(in, val))
            return;
        map[key] = val;
        key.clear();
        val.clear();
    }
}

void redis_api::write_map_to(std::unordered_map<std::string, std::string>& map, std::ostream& out) {
    for (auto record: map) {
        write_string(record.first, out);
        write_string(record.second, out);
    }
}

void redis_api::read_time_log(std::unordered_map<std::string, std::pair<time_t, time_t> >& map, std::istream& in) {
    std::string key;
    time_t start, time;
    while (in >> key >> start >> time) {
        map[key] = {start, time};
    }
}

void redis_api::write_time_log(std::unordered_map<std::string, std::pair<time_t, time_t> > &map, std::ostream &out) {
    for (auto record : map)
        out << record.first << "\t" << record.second.first << "\t" << record.second.second << std::endl;
}
void redis_api::show_map(std::ostream &out) {
    write_map_to(data, out);
}

void redis_api::save_chng() {
    apply_deleted();
    upd_change_log();
}

std::string redis_api::set(std::string key, std::string value, time_t time) {
    data[key] = value;
    change_map.insert({key, value});

    key_time[key] = {std::time(0), time};
    change_time[key] = {std::time(0), time};

    return "OK";
}

bool redis_api::valid_key(std::string key) {
    if (key_time.find(key) == key_time.end())
        return true;
    auto kt = key_time[key];
    return std::time(0) <= kt.first + kt.second;
}

void redis_api::delete_key(std::string key) {
    data.erase(key);
    key_time.erase(key);
    change_map.erase(key);
    change_time.erase(key);
}

void redis_api::apply_deleted() {
    for (auto key : deleted) {
        delete_key(key);
    }
}

void redis_api::write_string(const std::string& str, std::ostream& out) {
    int len = str.size();
    out.write((char*)&len, sizeof(len));
    out.write(str.c_str(), len);
    out.flush();
}

bool redis_api::read_string(std::istream& in, std::string& str) {
    int len;
    in.read((char*)&len, sizeof(len));
    if (!in)
        return false;
    char *buff = new char[len];
    in.read(buff, len);
    if (!in)
        return false;
    for (int i = 0; i < len; ++i)
        str.push_back(buff[i]);
    delete[] buff;
    return true;
}

