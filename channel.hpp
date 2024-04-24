#ifndef CHANNEL_HPP
#define CHANNEL_HPP


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
#include <string>

class Channel {
private:
    std::string Channelname;
    std::string topic;
    std::string key;
    std::string pass;
    int limite;
    std::map<std::string, int> userFdMap; // Mapping of usernames to file descriptors
    std::map<std::string, int> invitedUsers;
    std::map<std::string, int>  operators;
    int opperatorfd;
    bool issettop;
    bool isinveted;

public:

    Channel();
    Channel(const std::string& name);
    ~Channel();
    void setTopic(const std::string& newTopic);
    std::string getTopic() const;

    //checks
    void setoperator(int value);
    int getoperator();

    //check 
    void setlimitchannel(int value);
    int getlimitechannel();

    //checks
    void setbooltopic(bool value);
    bool getbooltopic();

    //checks
    void setboolinvited(bool value);
    bool getboolinvited();


    void addClient(const std::string& client, int fd);
    void addClientinveted(const std::string& client, int fd);
    void addOperator(const std::string& operatorName, int fd);
    int getUserFd(const std::string& username) const;
    std::vector<std::string> getClients() const;
    std::string getNickname(int fd) const;
    bool isOperator(int fd);
    bool isInvited(std::string nickname);
    int findUserFdForKickRegulars(const std::string& username);
    void ejectUserfromusers(int fd);
    void ejectUserfromivited(std::string nickname);
    std::string getOperatorNickname(int fd) const;
    void removeOperator(const std::string& operatorName );
    

    void setPass(const std::string &Newpass)
    {
        pass = Newpass;
    }

    std::string getPass()
    {
        return pass;
    }
};




#endif