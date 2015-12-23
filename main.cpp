#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include "redis_api.h"
#include "redis_server.h"

int main(int argc, char *argv[]) {
    char log[] = "log";
    char *data = log;
    if (argc != 1)
        data = argv[1];
    redis_api redis(data);
    Redis_server server(6379, &redis);

    while (1) {
        server.get_requests();
    }

    return 0;
}

