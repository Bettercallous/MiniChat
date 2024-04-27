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
#include <ctime>
#include <ctime>
#include <iomanip>
#define BUFFER_SIZE 1024
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

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
        std::string formatCreationTime();
        std::string constructCreationTimeMessage(const std::string& channelName);
        std::string constructJoinedTimeMessage(const std::string& channelName);
        void setUsernameregular(int fd, const std::string& username);
        void createChannel(const std::string& channel, const std::string& nickname, int fd);
        void handlePrivateMessage(int senderFd, const std::string& recipient, const std::string& message);
        void handleInvitation(int senderFd, const std::string& recipient, std::string channelName);
        void broadcastMessage(const std::string& channel, const std::string& senderNickname, const std::string& msg, int fd);
        void smallbroadcastMessagefortheckick(std::string nicknamesender , const std::string& channelname, const std::string& usertokick, const std::string& reason);
        void smallbroadcastMessageforjoin(std::string nicknamesender , const std::string& channelname);
        void smallbroadcastMessageforTopic(std::string nicknamesender, const std::string& channelname, std::string topic);
        void smallbroadcastMOOD(std::string nicknamesender, const std::string& channelname, std::string mode, std::string receiver);
        int findUserFd1(const std::string& username);
        std::string findUsernameforsending(int fd);
        bool isOperator(int fd);
        void kickUser(int fd);
        int findUserFdforkickregulars(const std::string& username);
        bool dontputthesamenick(const std::string& nickname);
        bool dontputthesameusername(const std::string& username);
        // AND END HERE.
        void parseArgs(int ac, char **av);
        static void receiveSignal(int signum);
        void init();
        void run();

        void createServerSocket();
        void bindServerSocket();
        void addPollfd(int fd);
        void handleClientConnection();
        void handleClientData(int fd);
        void clientCleanup(int fd);
        void closeFds();
      
};

int randomInRange(int min, int max);
std::string intToString(int number);

#endif