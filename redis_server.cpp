#include "redis_server.h"

Redis_server::Redis_server() {

}

Redis_server::~Redis_server() {

}

Redis_server::Redis_server(int port, redis_api* _r) {
    redis = _r;
    listener = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    listen(listener, 1);
}

void Redis_server::get_requests() {
    int sock = accept(listener, nullptr, nullptr);
    std::string request;
    while(full_req_read(sock, request)) {
        std::stringstream in(request);
        std::string command;
        in >> command;
        if (command == "SET") {
            std::string key, val;
            in >> key >> val;
            redis->set(key, val);
        } else if (command == "GET") {
            std::string key;
            in >> key;
            std::string ans = redis->get(key);
            full_req_write(sock, ans);
        } else if (command == "show") {
            std::stringstream stream;
            redis->show_map(stream);
            stream.seekp(0);
            std::string str;
            while (std::getline(stream, str)) {
                full_req_write(sock, str);
            }
            str = "";
            full_req_write(sock, str);
        } else if (command == "exit") {
            break;
        }
    }
    close(sock);
}

bool Redis_server::full_req_read(int sd, std::string &req) {
    char* buff = new char[4];
    int rd_size = recv(sd, buff, 4, 0);
    if (rd_size <= 0)
        return false;
    while ((rd_size += recv(sd, buff + rd_size, 4 - rd_size, 0)) != 4) {}
    int need_size = *reinterpret_cast<int *>(buff);
    char *big_buff = new char[need_size];
    rd_size = recv(sd, big_buff, need_size, 0);
    while (rd_size != need_size) {
        rd_size += recv(sd, big_buff + rd_size, need_size - rd_size, 0);
    }

    req = std::string(big_buff);
    req = req.substr(0, need_size) + "\0";

    return true;
}

bool Redis_server::full_req_write(int sd, std::string &req) {
    int size = req.size();
    int *size_ptr = &size;
    const char *buff = reinterpret_cast<const char *>(size_ptr);
    int send_size = send(sd, buff, 4, 0);
    while ((send_size += send(sd, buff + send_size, 4 - send_size, 0)) != 4) {}
    buff = req.c_str();
    send_size = send(sd, buff, req.size(), 0);
    while ((send_size += send(sd, buff + send_size, req.size() - send_size, 0)) != req.size()) {}

    return true;
}
