#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <csignal>

class Server {
    private:
        int _port;
        static bool _signal;
        std::string _password;
    public:
        Server();
        ~Server();

        void parseArgs(int ac, char **av);
        static void receiveSignal(int signum);
        void init();

};

#endif