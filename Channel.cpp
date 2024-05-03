#include "Channel.hpp"

Channel::Channel(){}

Channel::Channel(const std::string& name) : Channelname(name) {}

Channel::~Channel() {}

void Channel::setlimitchannel(int value) {limit = value;}

int Channel::getChannelLimit() {return limit;}

void Channel::setTopic(const std::string& newTopic) {topic = newTopic;}

std::string Channel::getTopic() const {return topic;}

void Channel::addClient(const std::string& client, int fd) {userFdMap[client] = fd;}

void Channel::addClientinveted(const std::string& client, int fd) {invitedUsers[client] = fd;}

void Channel::addOperator(const std::string& operatorName, int fd) {operators[operatorName] = fd;}

std::map<std::string, int>& Channel::getUserFdMap() {return userFdMap;}

std::map<std::string, int>& Channel::invitedUserss() {return invitedUsers;}

std::map<std::string, int>& Channel::getOperators() {return operators;}

void Channel::setPass(const std::string &Newpass) {pass = Newpass;}

std::string Channel::getPass() {return pass;}

int Channel::getUserFd(const std::string& username) const {
    std::map<std::string, int>::const_iterator it = userFdMap.find(username);
    if (it != userFdMap.end()) {
        return it->second;
    }
    return -1; 
}

bool Channel::isUserInChannel(const std::string& nickname) const {
    std::map<std::string, int>::const_iterator it = userFdMap.find(nickname);
    if (it != userFdMap.end() && it->second) {
        return true;
    }
    return false;
}

std::vector<std::string> Channel::getClients() const {
    std::vector<std::string> clients;
    std::map<std::string, int>::const_iterator it;
    for (it = userFdMap.begin(); it != userFdMap.end(); ++it) {
        clients.push_back(it->first);
    }
    return clients;
}

std::string Channel::getNickname(int fd) const {
    std::map<std::string, int>::const_iterator it;
    for (it = userFdMap.begin(); it != userFdMap.end(); ++it) {
    if (it->second == fd) {
        return it->first;
        }
    }
    return ""; 
}


bool Channel::isOperator(int fd) {
    for (std::map<std::string, int>::iterator it = operators.begin(); it != operators.end(); ++it) {
        if (it->second == fd) {
            return true; 
        }
    }
    return false; 
}

bool Channel::isInvited(std::string nickname) {
    for (std::map<std::string, int>::iterator it = invitedUsers.begin(); it != invitedUsers.end(); ++it) {
        if (it->first == nickname) {
            return true;
        }
    }
    return false;
}

int Channel::findUserFdForKickRegulars(const std::string& username) {
    std::map<std::string, int>::iterator it;
    for (it = userFdMap.begin(); it != userFdMap.end(); ++it) {
        if (it->first == username) {
            return it->second;
        }
    }
    return -1;
}

void Channel::ejectUserfromusers(int fd) {
   std::map<std::string, int>::iterator it;
   for (it = userFdMap.begin(); it != userFdMap.end(); ++it) {
       if (it->second == fd) {
           userFdMap.erase(it);
           return; 
       }
   }
}

void Channel::ejectUserfromivited(std::string nickname) {
   std::map<std::string, int>::iterator it;
   for (it = invitedUsers.begin(); it != invitedUsers.end(); ++it) {
       if (it->first == nickname) {
           invitedUsers.erase(it);
           return;
       }
   }
}

std::string Channel::getOperatorNickname(int fd) const {
    std::map<std::string, int>::const_iterator it;
    for (it = operators.begin(); it != operators.end(); ++it) {
        if (it->second == fd) {
            return it->first;
        }
    }
    return "";
}

void Channel::removeOperator(const std::string& operatorName )
{
    std::map<std::string, int>::iterator it;
    for (it = operators.begin(); it != operators.end(); ++it) {
        if (it->first == operatorName) {
            operators.erase(it);
            return; 
        }
    }
}
