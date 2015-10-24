#ifndef REDIS_API_H
#define REDIS_API_H

#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>

class redis_api {
private:
    std::unordered_map<std::string, std::string> data, change_map;
    std::fstream log, change_log;
    std::string log_name;

    void read_map_from(std::unordered_map<std::string, std::string>& map, std::istream& in);
    void write_map_to(std::unordered_map<std::string, std::string>& map, std::ostream& out);
    void upd_change_log();
    void apply_change_to_log();

public:
    explicit redis_api(std::string file);
    explicit redis_api();

    ~redis_api();

    int set(std::string key, std::string value);
    std::string get(std::string key);
    void show_map(std::ostream&);

};

#endif // REDIS_API_H
