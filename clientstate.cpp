#include "clientstate.h"


Client_State::Client_State(int _sd): sd(_sd), BUF_SIZE(4096) {
    init();

//    state_fun[0] = boost::lambda::bind(&Client_State::Arr_state, );
}

Client_State::Client_State(): sd(-1), BUF_SIZE(4096) {
    init();
}

Client_State::~Client_State() {

}

bool Client_State::get_next_char(char& c) {
    if (buf.empty())
        return false;
    c = buf.front();
    buf.pop_front();
    return true;
}

void Client_State::init() {
    arr.clear();
    str.clear();
    arr_len = str_len = 0;
    state = Start;
    back_r = false;
}

void Client_State::Start_state(char symb) {
    init();
    switch (symb) {
    case '*':
        arr_len = 0;
        state = Arr_Length;
        break;
    default:
        state = Error_State;
        break;
    }
}

void Client_State::Arr_Length_state(char symb) {
    state = Arr_Length;

    if (symb <= '9' && symb >= '0') {
        arr_len *= 10;
        arr_len += symb - '0';
        state = Arr_Length;
        return;
    }

    if (symb == '\r') {
        back_r = true;
        state = Arr_Length;
        return;
    }

    if (symb == '\n' && back_r) {
        back_r = false;
        state = Arr;
        return;
    }

    state = Error_State;
}

void Client_State::Arr_state(char symb) {
    state = Arr;

    switch (symb) {
    case '$':
        if (arr_len != 0) {
            str_len = 0;
            state = String_Length;
            return;
        } else {
            state = Ok_State;
            return;
        }
        break;
    default:
        state = Error_State;
        return;
        break;
    }
}

void Client_State::String_Length_state(char symb) {
    state = String_Length;

    if (symb <= '9' && symb >= '0') {
        str_len *= 10;
        str_len += symb - '0';
        state = String_Length;
        return;
    }

    if (symb == '\r') {
        back_r = true;
        state = String_Length;
        return;
    }

    if (symb == '\n' && back_r) {
        back_r = false;
        state = String;
        return;
    }

    state = Error_State;
    return;
}


void Client_State::String_state(char symb) {
    state = String;

    if (str_len != 0) {
        str.push_back(symb);
        --str_len;
        state = String;
        return;
        return;
    }

    if (symb == '\r') {
        back_r = true;
        state = String;
        return;
    }

    if (symb == '\n' && back_r) {
        back_r = false;
        arr.push_back(str);
        str.clear();
        --arr_len;
        if (arr_len == 0) {
            state = Ok_State;
            return;
        } else {
            state = Arr;
            return;
        }
    } else {
        state = Error_State;
        return;
    }
}


void Client_State::Error_State_state() {
    state = Error_State;
}

void Client_State::OK_state() {
    state = Ok_State;
}

void Client_State::read_buf() {
    char buff[BUF_SIZE];
    while (true) {
        int size = 0;
        size = recv(sd, buff, BUF_SIZE, 0);
        if (size > 0) {
            for (int i = 0; i < size; ++i)
                buf.push_back(buff[i]);
        } else if (size == 0) {
            state = Refuse;
            return;
        } else {
            break;
        }
    }
}

Client_State::Result Client_State::get_request(std::vector<std::string>& req) {
    parse();
    switch (state) {
    case Ok_State:
        req = arr;
        state = Start;
        return OK;
        break;
    case Error_State:
        return ERROR;
        state = Start;
        break;
    case Refuse:
        return REFUSE;
    default:
        return WAIT;
        break;
    }
}

void Client_State::parse() {
    read_buf();
    char symb;
    while (get_next_char(symb)) {
        switch (state) {
        case Start:
            Start_state(symb);
            break;
        case Arr:
            Arr_state(symb);
            break;
        case Arr_Length:
            Arr_Length_state(symb);
            break;
        case String:
            String_state(symb);
            break;
        case String_Length:
            String_Length_state(symb);
            break;
        case Ok_State:
            return;
            break;
        case Error_State:
            return;
            break;
        default:
            return;
            break;
        }
        if (state == Ok_State)
            return;

    }
}

bool Client_State::empty() {
    return buf.empty();
}
