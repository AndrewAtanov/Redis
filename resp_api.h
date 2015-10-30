#ifndef RESP_API_H
#define RESP_API_H

#include <vector>
#include <string>

void arr_to_str(std::vector<std::string>& arr, std::string& str);
int str_to_arr(std::string& str, std::vector<std::string>& arr);

bool full_req_write(int sd, std::string &str);
bool full_req_read(int sd, std::string &req);
bool get_string(int sd, std::string& ans, int n);
bool get_arr(int sd, std::string& ans, int n);
bool readnbytes(int sd, int n, char* buff);
int req_to_string(const char* req, std::string& ans);



#endif // RESP_API_H
