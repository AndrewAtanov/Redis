#include "redis_api.h"

#include <ctime>

redis_api::redis_api(std::string file):    
    log_name(file) {
    std::ifstream get_data(file.c_str());
    read_map_from(data, get_data);
    get_data.close();

    log.open(file.c_str(), std::ios::out | std::ios::in);
    change_log.open("change_log", std::ios::in | std::ios::out);

    apply_change_to_log();
}

redis_api::redis_api() {

}


redis_api::~redis_api() {
    log.close();
    change_log.close();
}

int redis_api::set(std::string key, std::string value) {
    data[key] = value;
    change_map.insert({key, value});
    upd_change_log();

    return 0;
}

std::string redis_api::get(std::string key) {
    auto it = data.find(key);
    if (it != data.end())
        return data[key];
    else
        return "No such value";
}

void redis_api::upd_change_log() {
    write_map_to(change_map, change_log);
    change_map.clear();
    }

void redis_api::apply_change_to_log() {
    read_map_from(data, change_log);
    std::ofstream tmp_log("tmp_log");
    write_map_to(data, tmp_log);
    std::string comand = "cp tmp_log " + log_name;
    system(comand.c_str());
    comand = "rm tmp_log";
    system(comand.c_str());

    change_log.close();
    change_log.open("change_log", std::ios::in | std::ios::out | std::ios::trunc);
}

void redis_api::read_map_from(std::unordered_map<std::string, std::string>& map, std::istream &in) {
    std::string key, value;
    while (in >> key >> value)
        map[key] = value;
}

void redis_api::write_map_to(std::unordered_map<std::string, std::string>& map, std::ostream& out) {
    for (auto record: map)
        out << record.first << '\t' << record.second << std::endl;
}

void redis_api::show_map(std::ostream &out) {
    write_map_to(data, out);
}
