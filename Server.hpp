#ifndef SERVER_HPP
#define SERVER_HPP

#include <sstream>
#include <algorithm>
#include <cctype>
#include <unistd.h>
#include <cstring>
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
#include "Channel.hpp"
#include <ctime>
#include <ctime>
#include <iomanip>

class Server {
    private:
        int _port;
        int _serverSocketFd;
        static bool _signal;
        std::string _password;
        std::vector<struct pollfd> _fds;
        std::vector<Client> _clients;
        std::map<int, std::string> nicknames;
        std::map<int, std::string> usernames;
        std::map<std::string, Channel> channels;

    public:
        Server();
        ~Server();
    
        std::string getPassowrd() const;
        Client& getClientByFd(int fd);

        void setNickname(int fd, const std::string& nickname);
        void setUsernames(int fd, const std::string& username);
        void setUsernameregular(int fd, const std::string& username);
    
        std::string formatCreationTime();
        std::string constructCreationTimeMessage(const std::string& channelName);
        std::string constructJoinedTimeMessage(const std::string& channelName);
        
        void createChannel(const std::string& channel, const std::string& nickname, int fd);
        void handlePrivateMessage(int senderFd, const std::string& recipient, const std::string& message);
        void handleInvitation(int senderFd, const std::string& recipient, std::string channelName);
        
        void broadcastMessage(const std::string& channel, const std::string& senderNickname, const std::string& msg);
        void smallbroadcastMessagefortheckick(std::string nicknamesender , const std::string& channelname, const std::string& usertokick, const std::string& reason);
        void smallbroadcastMessageforjoin(std::string nicknamesender , const std::string& channelname);
        void smallbroadcastMessageforTopic(std::string nicknamesender, const std::string& channelname, std::string topic);
        void smallbroadcastMOOD(std::string nicknamesender, const std::string& channelname, std::string mode, std::string receiver);
        int findUserFd1(const std::string& username);
        bool dontputthesamenick(const std::string& nickname);
        bool dontputthesameusername(const std::string& username);

        // Server
        void init();
        void run();
        void parseArgs(int ac, char **av);
        static void receiveSignal(int signum);
        void createServerSocket();
        void bindServerSocket();
        void addPollfd(int fd);
        void handleClientConnection();
        void handleClientData(int fd);
        void clientCleanup(int fd);
        void cleanChannel(int fd);
        void closeFds();

        void addUserToChannel(const std::string& nickname, const std::string& channelName, int fd);
        void sendJoinMsg(const std::string& nickname, const std::string& channelName, int fd);
        void ping(const std::string& command, int fd);

        // User authentication
        void processPassword(Client& client, const std::string& command, int fd);
        void processNickCmd(Client& client, const std::string& command, int fd);
        void processUserCmd(Client& client, const std::string& command, int fd);
        void welcome(const std::string& nickname, int fd);
        
        // Commands
        void processJoinCmd(Client& client, const std::string& command, int fd);
        void processPrivmsgCmd(Client& client, const std::string& command, int fd);
        void processQuitCmd(int fd);
        void processKickCmd(Client& client, const std::string& command, int fd);
        void processTopicCmd(Client& client, const std::string& command, int fd);
        void processInviteCmd(Client& client, const std::string& command, int fd);
        void processModeCmd(Client& client, const std::string& command, int fd);
        void processBotCmd(Client& client, const std::string& command, int fd);

        // Channel modes
        void handleOpPrivilege(const std::string& nick, const std::string& channelName, const std::string& mode, int fd);
        void handleTopicRestriction(const std::string& nick, const std::string& channelName, const std::string& mode, int fd);
        void handleInviteOnly(const std::string& nick, const std::string& channelName, const std::string& mode, int fd);
        void handleChannelKey(std::string& nick, const std::string& channelName, const std::string& mode, int fd);
        void handleChannelLimit(const std::string& nick, const std::string& channelName, const std::string& mode, int fd);
};

int randomInRange(int min, int max);
std::string intToString(int number);

#endif