#ifndef REDIS_API_H
#define REDIS_API_H

#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <set>

class redis_api {
private:
    std::unordered_map<std::string, std::string> data, change_map;
    std::fstream log, change_log, time_log, change_time_log;
    std::string log_name;
    std::unordered_map<std::string, std::pair<time_t, time_t>> key_time, change_time;
    std::set<std::string> deleted;

    void read_map_from(std::unordered_map<std::string, std::string>& map, std::istream& in);
    void write_map_to(std::unordered_map<std::string, std::string>& map, std::ostream& out);
    void upd_change_log();
    void apply_change_to_log();
    void delete_key(std::string key);
    void apply_deleted();
    void write_string(const std::string &str, std::ostream &out);
    std::string read_string(std::istream& in);

    void read_time_log(std::unordered_map<std::string, std::pair<time_t, time_t>>& map, std::istream& in);
    void write_time_log(std::unordered_map<std::string, std::pair<time_t, time_t>>& mpa, std::ostream& out);

public:
    explicit redis_api(std::string file);
    explicit redis_api();

    ~redis_api();

    std::string set(std::string key, std::string value);
    std::string set(std::string key, std::string value, time_t time);
    std::string get(std::string key);
    void show_map(std::ostream&);
    void save_chng();
    bool valid_key(std::string key);

};

#endif // REDIS_API_H
