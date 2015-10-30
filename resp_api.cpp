#include "resp_api.h"

#include <sys/socket.h>
#include <cstring>

bool string_to_req(std::string& str, std::string& req) {
    req = "$" + std::to_string(str.size()) + "\r\n" + str + "\r\n";
    return true;
}

void arr_to_str(std::vector<std::string>& arr, std::string& str) {
    str = "*" + std::to_string(arr.size()) + "\r\n";
    for (auto el: arr)
        str += "$" + std::to_string(el.size()) + "\r\n" + el + "\r\n";
}

int str_to_arr(std::string& str, std::vector<std::string>& arr) {
    size_t ind;
    int size = std::stoll(str.c_str() + 1, &ind);
    ind += 3;
    for (int i = 0; i < size; ++i) {
        std::string el;
        ind += req_to_string(str.c_str() + ind, el);
        arr.push_back(el);
    }
    return ind;
}

int req_to_string(const char* req, std::string& ans) {
    size_t ind;
    int size = std::stoll(req + 1, &ind);
    ind += 3;
    char buff[size];
    std::strncpy(buff, req + ind, size);
    ans = buff;
    ans.resize(size);
    return ind + size + 2;
}

bool full_req_write(int sd, std::string& str) {
//    int size = str.size();
//    int *size_ptr = &size;
//    const char *buff = reinterpret_cast<const char *>(size_ptr);
//    int send_size = send(sd, buff, 4, 0);
//    while ((send_size += send(sd, buff + send_size, 4 - send_size, 0)) != 4) {}
    const char *buff = str.c_str();
    int send_size = send(sd, buff, str.size(), 0);
    while ((send_size += send(sd, buff + send_size, str.size() - send_size, 0)) != str.size()) {}

    return true;
}

bool readnbytes(int sd, int n, char* buff) {
    int rs = recv(sd, buff, n, 0);
    if (rs <= 0)
        return false;
    while (rs != n)
        rs += recv(sd, buff, n - rs, 0);
    return true;
}

bool get_type(int sd, char* buff) {
    return readnbytes(sd, 1, buff);
}

bool get_size(int sd, char* buff) {
    int size = 0;
    do {
        if (!readnbytes(sd, 1, buff + size++))
            return false;
    } while (*(buff + size - 2) != '\r' || *(buff + size - 1) != '\n');
    return true;
}

bool get_param(int sd, std::pair<char, long long>& ans) {
    size_t len = 1;
    char* buff = new char[1024];
    if (!get_type(sd, buff))
        return false;
    if (!get_size(sd, buff + 1))
        return false;
    ans = std::pair<char, size_t>{buff[0], std::stoull(buff + 1)};
    return true;
}

bool get_arr(int sd, std::string& ans, int n) {
    for (int i = 0; i < n; ++i) {
        std::pair<char, long long> param;
        get_param(sd, param);
        ans.push_back(param.first);
        ans += std::to_string(param.second) + "\r\n";
        get_string(sd, ans, param.second);
    }
    return true;
}

bool get_string(int sd, std::string& ans, int n) {
    char *buff = new char[n + 3];
    buff[n + 2] = '\0';
    readnbytes(sd, n + 2, buff);
    std::string str = std::string(buff);
    str.resize(n + 2);
    ans += str;
    delete[] buff;
}

bool full_req_read(int sd, std::string &req) {
    std::pair<char, long long> param;
    if (!get_param(sd, param))
        return false;

    req.push_back(param.first);
    req += std::to_string(param.second) + "\r\n";

    if (param.first == '*')
        get_arr(sd, req, param.second);
    else if (param.first == '$')
        get_string(sd, req, param.second);
    else
        return false;

    return true;
}
