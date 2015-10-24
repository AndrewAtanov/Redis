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
    Redis_server server(3425, &redis);

    while (1) {
        server.get_requests();
    }


//    int sock, listener;
//    struct sockaddr_in addr;
//    char buf[1024];
//    int bytes_read;
//    listener = socket(AF_INET, SOCK_STREAM, 0);

//    addr.sin_family = AF_INET;
//    addr.sin_port = htons(3425);
//    addr.sin_addr.s_addr = htonl(INADDR_ANY);

//    bind(listener, (struct sockaddr *)&addr, sizeof(addr));


//    listen(listener, 10);

//    while (1) {
//        sock = accept(listener, NULL, NULL);
//        std::string request;
//        while(full_rec_read(sock, request)) {
//            std::stringstream in(request);
//            std::string command;
//            in >> command;
//            if (command == "SET") {
//                std::string key, val;
//                in >> key >> val;
//                redis.set(key, val);
//            } else if (command == "GET") {
//                std::string key;
//                in >> key;
//                std::cout << redis.get(key) << std::endl;
//            } else if (command == "show") {
//                redis.show_map();
//            } else if (command == "exit") {
//                break;
//            }
//        }
////        close(sock);
//    }



    //    char log[] = "log";
    //    char *data = log;
    //    if (argc != 1)
    //        data = argv[1];
    //    redis_api redis(data);
    //    std::string redis_s = "Redis> ";
    //    std::string comand;
    //    std::cout << redis_s;
    //    while (std::cin >> comand) {
    //        if (comand == "SET") {
    //            std::string key, val;
    //            std::cin >> key >> val;
    //            redis.set(key, val);
    //        } else if (comand == "GET") {
    //            std::string key;
    //            std::cin >> key;
    //            std::cout << redis.get(key) << std::endl;
    //        } else if (comand == "show") {
    //            redis.show_map();
    //        } else if (comand == "exit") {
    //            break;
    //        }
    //        std::cout << redis_s;
    //    }
    return 0;
}

