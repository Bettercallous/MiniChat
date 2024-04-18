#ifndef SERVER_HPP
#define SERVER_HPP

#include <sstream>
#include <algorithm>
#include <cctype>
#include <unistd.h> // Include for the send function
#include <cstring> // Include for the strlen function
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include "Client.hpp"
#include <cstring>
#include <map>
#include "channel.hpp"
#define BUFFER_SIZE 1024


class Server {
    private:
        int _port;
        int _serverSocketFd;
        static bool _signal;
        std::string _password;
        std::vector<struct pollfd> _fds;
        std::vector<Client> _clients;
        // THAT'S THA DATA OF TOOOP GGG START FROM THERE .
        std::map<int, std::string> nicknames; // Replace unordered_map with map
        std::map<int, std::string> usernamesoperators; // Replace unordered_map with map

        std::map<int, std::string> usernames; // Replace unordered_map with map
        std::map<int, std::string> usernamesregulars;
        // std::map<std::string, std::vector<std::string> > channels; //here a chanel name and list of client in every chanel 
        std::map<std::string, Channel> channels;

        

    public:
        Server();
        ~Server();
        // THAT'S MY FUNCTIONS START FROM THERE
        void setNickname(int fd, const std::string& nickname);
        std::string getPassowrd() const;
        void setPassword(const std::string& password); 
        void setUsernameoperators(int fd, const std::string& username);
        void setUsernames(int fd, const std::string& username);

        void setUsernameregular(int fd, const std::string& username);
        void createChannel(const std::string& channel, const std::string& nickname, int fd);
        void handlePrivateMessage(int senderFd, const std::string& recipient, const std::string& message);
        void broadcastMessage(const std::string& channel, const std::string& senderNickname, const std::string& msg);
        int findUserFd1(const std::string& username);
        std::string findUsernameforsending(int fd);
        bool isOperator(int fd);
        void kickUser(int fd);
        int findUserFdforkickregulars(const std::string& username);
        // AND END HERE.
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
        void closeFds();

        void debugPrintChannels() {
    std::cout << "List of channels:" << std::endl;
    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it) {
        std::cout << "- " << it->first << std::endl; // Print the channel name (it->first)
    }
}
};

#endif