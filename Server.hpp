#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <csignal>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>

class Server {
    private:
        int _port;
        int _serverSocketFd;
        static bool _signal;
        std::string _password;
        std::vector<struct pollfd> _fds;
    public:
        Server();
        ~Server();

        void parseArgs(int ac, char **av);
        static void receiveSignal(int signum);
        void init();

        void createServerSocket();
        void bindServerSocket();
        void addPollfd(int fd, short events, short revents);

};

#endif