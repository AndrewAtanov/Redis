#include "redis_server.h"

Redis_server::Redis_server() {

}

Redis_server::~Redis_server() {

}

Redis_server::Redis_server(int port, redis_api* _r) {
    redis = _r;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listener, F_SETFL, O_NONBLOCK);

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    listen(listener, 2);
}

void Redis_server::save_chng() {
    time_t cur_time = time(0);
    if (cur_time - prev_time >= 2) {
        redis->save_chng();
        cur_time = prev_time;
    }
}

int Redis_server::get_requests() {
    save_chng();
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(listener, &read_set);

    timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int max_fd = listener;

    for (int fd : clients) {
        FD_SET(fd, &read_set);
        if (fd > max_fd)
            max_fd = fd;
    }

    int result = select(max_fd + 1, &read_set, NULL, NULL, &timeout);
    if (result <= 0) {
        return 1;
    }


    if(FD_ISSET(listener, &read_set)) {
        // Поступил новый запрос на соединение, используем accept
        int sock = accept(listener, NULL, NULL);
        if(sock < 0)
            return 2;

        fcntl(sock, F_SETFL, O_NONBLOCK);
        clients.insert(sock);
    }

    for(int sd : clients) {
        if(FD_ISSET(sd, &read_set)) {
            // Поступили данные от клиента, читаем их
            std::string str_req;
            if (full_req_read(sd, str_req)) {
                std::string resp = process_request(str_req);
                if (resp == "exit") {
                    close(sd);
                    clients.erase(sd);
                    continue;
                }
                full_req_write(sd, resp);
            } else {
                // Соединение разорвано, удаляем сокет из множества
                close(sd);
                clients.erase(sd);
                continue;
            }
        }
    }

//    int sock = accept(listener, nullptr, nullptr);
//    while (1) {
//        if (!full_req_read(sock, str_req))
//            break;
//        std::string resp = process_request(str_req);
//        if (resp == "exit")
//            break;
//        full_req_write(sock, resp);
//    }
//    close(sock);

    return 0;
}

std::string Redis_server::process_request(std::string &str_req) {
    std::vector<std::string> req_arr;
    std::string resp;

    str_to_arr(str_req, req_arr);
    req_arr[0];
    if (req_arr[0] == "SET") {
        std::string ans = redis->set(req_arr[1], req_arr[2]);
        string_to_req(ans, resp);
    } else if (req_arr[0] == "GET") {
        std::string ans = redis->get(req_arr[1]);
        string_to_req(ans, resp);
    } else if (req_arr[0] == "show") {
        std::string ans = "NO";
        string_to_req(ans, resp);
    } else if (req_arr[0] == "exit") {
        resp == "exit";
    }

    return resp;
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
