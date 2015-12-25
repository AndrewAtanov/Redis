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
        clients_state.insert({sock, Client_State(sock)});
    }

    std::set<int> refused;
    for(int sd : clients) {
        if(FD_ISSET(sd, &read_set) && sd != listener) {
            // Поступили данные от клиента, читаем их

            std::vector<std::string> req;
            Client_State::Result pars_result = clients_state[sd].get_request(req);
            if (pars_result == Client_State::ERROR) {
                refused.insert(sd);
            }

            if (pars_result == Client_State::OK) {
                std::string resp = process_request(req);
                if (resp == "exit") {
                    refused.insert(sd);

                    continue;
                }
                if (resp == "break") {
                    refused.insert(sd);

                    continue;
                }
                full_req_write(sd, resp);
            }

            if (pars_result == Client_State::REFUSE) {
                refused.insert(sd);

            }
        }
    }

    for (int sd : refused) {
        break_connection(sd);
    }

    return 0;
}

std::string Redis_server::process_request(std::vector<std::string> &req_arr) {
    std::string resp = "break";
    if (req_arr[0] == "SET") {
        if (req_arr.size() == 3) {
            std::string ans = redis->set(req_arr[1], req_arr[2]);
            string_to_req(ans, resp);
        } else if (req_arr.size() == 5 && req_arr[3] == "EX") {
            std::string ans = redis->set(req_arr[1], req_arr[2], std::stoull(req_arr[4]));
            string_to_req(ans, resp);
        }
    } else if (req_arr[0] == "GET") {
        if (req_arr.size() == 2) {
            std::string ans = redis->get(req_arr[1]);
            string_to_req(ans, resp);
        }
    } else if (req_arr[0] == "show") {
        std::string ans = "NO";
        string_to_req(ans, resp);
    } else if (req_arr[0] == "exit") {
        resp = "exit";
    } else {
        resp = "break";
    }

    return resp;
}

void Redis_server::break_connection(int sd) {
    close(sd);
    clients.erase(sd);
    clients_state.erase(sd);
}
