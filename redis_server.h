#ifndef REDIS_SERVER_H
#define REDIS_SERVER_H

#include "redis_api.h"
#include "resp_api.h"

#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <set>
#include <algorithm>
#include <fcntl.h>
#include <time.h>

class Redis_server {
private:
    time_t prev_time;
    int listener;
    redis_api *redis;
    std::set<int> clients;
//    bool full_req_read(int sd, std::string& req);
//    bool full_req_write(int sd, std::string& req);
public:
    Redis_server(int port, redis_api *_r);
    Redis_server();
    ~Redis_server();

    int get_requests();
    std::string process_request(std::string& str_req);
    void save_chng();
};

#endif // REDIS_SERVER_H
