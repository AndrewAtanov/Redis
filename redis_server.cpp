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
    listen(listener, 2);
}

void Redis_server::get_requests() {
    int sock = accept(listener, nullptr, nullptr);
    while (1) {
        std::string str_req;
        if (!full_req_read(sock, str_req))
            break;
        std::vector<std::string> req_arr;
//        std::cout << str_req << std::endl;
            str_to_arr(str_req, req_arr);
        req_arr[0];
        if (req_arr[0] == "SET") {
            std::string ans = redis->set(req_arr[1], req_arr[2]);
            std::string resp;
            string_to_req(ans, resp);
            full_req_write(sock, resp);
        } else if (req_arr[0] == "GET") {
            std::string ans = redis->get(req_arr[1]);
            std::string resp;
            string_to_req(ans, resp);
            full_req_write(sock, resp);
        } else if (req_arr[0] == "show") {
            std::string ans = "NO";
            std::string resp;
            string_to_req(ans, resp);
            full_req_write(sock, resp);
        } else if (req_arr[0] == "exit") {
            break;
        }
    }
    close(sock);
}

//bool Redis_server::full_req_read(int sd, std::string &req) {
//    char* buff = new char[4];
//    int rd_size = recv(sd, buff, 4, 0);
//    if (rd_size <= 0)
//        return false;
//    while ((rd_size += recv(sd, buff + rd_size, 4 - rd_size, 0)) != 4) {}
//    int need_size = *reinterpret_cast<int *>(buff);
//    char *big_buff = new char[need_size];
//    rd_size = recv(sd, big_buff, need_size, 0);
//    while (rd_size != need_size) {
//        rd_size += recv(sd, big_buff + rd_size, need_size - rd_size, 0);
//    }

//    req = std::string(big_buff);
//    req = req.substr(0, need_size) + "\0";

//    return true;
//}

//bool Redis_server::full_req_write(int sd, std::string &req) {
//    int size = req.size();
//    int *size_ptr = &size;
//    const char *buff = reinterpret_cast<const char *>(size_ptr);
//    int send_size = send(sd, buff, 4, 0);
//    while ((send_size += send(sd, buff + send_size, 4 - send_size, 0)) != 4) {}
//    buff = req.c_str();
//    send_size = send(sd, buff, req.size(), 0);
//    while ((send_size += send(sd, buff + send_size, req.size() - send_size, 0)) != req.size()) {}

//    return true;
//}
