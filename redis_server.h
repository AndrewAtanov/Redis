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


class Redis_server {
private:
    int listener;
    redis_api *redis;
//    bool full_req_read(int sd, std::string& req);
//    bool full_req_write(int sd, std::string& req);
public:
    Redis_server(int port, redis_api *_r);
    Redis_server();
    ~Redis_server();

    void get_requests();
};

#endif // REDIS_SERVER_H
