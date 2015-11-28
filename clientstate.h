#ifndef CLIENTSTATE_H
#define CLIENTSTATE_H

#include <string>
#include <deque>
#include <sys/socket.h>
#include <vector>
#include <map>
#include <boost/function.hpp>
#include <boost/lambda/bind.hpp>


class Client_State {
    enum State {Start, Arr, String, String_Length, Arr_Length, Error_State, Ok_State};
    typedef void (*SomeFunction)(void);
    typedef std::map<int, boost::function<void(void)>> script_map;

private:
    script_map state_fun;
    std::vector<std::string> arr;
    std::deque<char> buf;
    State state;
    std::string str;
    int sd;
    ssize_t str_len, arr_len;
    const int BUF_SIZE;
    bool back_r;


    void init();
    bool get_next_char(char&);
    void Start_state(char symb);
    void Arr_state(char symb);
    void String_state(char symb);
    void String_Length_state(char symb);
    void Arr_Length_state(char symb);
    void Error_State_state( );
    void OK_state();
public:
    enum Result {OK, ERROR, WAIT};
    Client_State(int _sd);
    Client_State();
    ~Client_State();

    void read_buf();
    Result get_request(std::vector<std::string> &req);
    void parse();

};

#endif // CLIENTSTATE_H
