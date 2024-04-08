#ifndef SERVER_HPP
#define SERVER_HPP


#include <iostream>
#include <cstdlib>

class Server {
    private:
        int _port;
        std::string _password;
    public:
        Server();
        ~Server();

        void parseArgs(int ac, char **av);
};

#endif