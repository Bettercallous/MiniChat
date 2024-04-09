#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <csignal>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include "Client.hpp"
#include <cstring>
#define BUFFER_SIZE 1024

class Server {
    private:
        int _port;
        int _serverSocketFd;
        static bool _signal;
        std::string _password;
        std::vector<struct pollfd> _fds;
        std::vector<Client> _clients;
    public:
        Server();
        ~Server();

        void parseArgs(int ac, char **av);
        static void receiveSignal(int signum);
        void init();
        void run();

        void createServerSocket();
        void bindServerSocket();
        void addPollfd(int fd, short events, short revents);
        void handleClientConnection();
        void handleClientData(int fd);
        void clientCleanup(int fd);
};

#endif