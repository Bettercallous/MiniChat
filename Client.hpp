#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client {
    private:
        int _fd;
        std::string _addr;
        bool _isRegistered;
    public:
        Client();
        Client(int fd, std::string addr);
        ~Client();

        int getFd() const;
};

#endif