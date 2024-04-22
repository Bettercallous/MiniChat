#include "channel.hpp"


Channel::Channel(){}

Channel::Channel(const std::string& name) : Channelname(name), opperatorfd(false), issettop(false), isinveted(false) {}

Channel::~Channel() {}


void Channel::setoperator(int value)
{
    opperatorfd = value;
}

int Channel::getoperator()
{
    return opperatorfd;
}

void Channel::setlimitchannel(int value)
{
    limite = value;
}

int Channel::getlimitechannel()
{
    return limite;
}

void Channel::setbooltopic(bool value)
{
    issettop = value;
}

bool Channel::getbooltopic()
{
    return issettop;
}

void Channel::setboolinvited(bool value)
{
    isinveted = value;
}

bool Channel::getboolinvited()
{
    return isinveted;
}

void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}

    // Get topic function
std::string Channel::getTopic() const {
    return topic;

}

void Channel::addClient(const std::string& client, int fd) {
    userFdMap[client] = fd;
}

void Channel::addClientinveted(const std::string& client, int fd) {
    invitedUsers[client] = fd;
}

    // Add an operator to the channel
void Channel::addOperator(const std::string& operatorName, int fd) {
// Store the operator name and file descriptor in the map
operators[operatorName] = fd;
}

int Channel::getUserFd(const std::string& username) const {
    std::map<std::string, int>::const_iterator it = userFdMap.find(username);
    if (it != userFdMap.end()) {
        return it->second;
    }
    return -1; // Return -1 if username not found
}

    // Get all clients' usernames in the channel
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
        return it->first; // Return the nickname if the file descriptor matches
    }
    }
    return ""; // Return an empty string if the file descriptor is not found
}


bool Channel::isOperator(int fd) {
// Iterate through the map of operators
for (std::map<std::string, int>::iterator it = operators.begin(); it != operators.end(); ++it) {
    // Check if the file descriptor matches
    if (it->second == fd) {
        return true; // Found the file descriptor in the map
    }
}
return false; // File descriptor not found in the map
}

bool Channel::isInvited(std::string nickname) {
// Iterate through the map of operators
for (std::map<std::string, int>::iterator it = invitedUsers.begin(); it != invitedUsers.end(); ++it) {
    // Check if the file descriptor matches
    if (it->first == nickname) {
        return true; // Found the file descriptor in the map
    }
}
return false; // File descriptor not found in the map
}

    

int Channel::findUserFdForKickRegulars(const std::string& username) {
// Iterate through the userFdMap to find the user
std::map<std::string, int>::iterator it;
for (it = userFdMap.begin(); it != userFdMap.end(); ++it) {
    if (it->first == username) {
        return it->second; // Return the file descriptor if the username matches
    }
}
return -1; // Return -1 if the user is not found
}


void Channel::ejectUserfromusers(int fd) {
   // Iterate over the map to find the user with the given file descriptor
   std::map<std::string, int>::iterator it;
   for (it = userFdMap.begin(); it != userFdMap.end(); ++it) {
       if (it->second == fd) {
           // Erase the user from the map
           userFdMap.erase(it);
           std::cout << "the user earased " << std::endl;
           return; // Exit the function after removing the user
       }
   }
}

void Channel::ejectUserfromivited(std::string nickname) {
   // Iterate over the map to find the user with the given file descriptor
   std::map<std::string, int>::iterator it;
   for (it = invitedUsers.begin(); it != invitedUsers.end(); ++it) {
       if (it->first == nickname) {
           // Erase the user from the map
           invitedUsers.erase(it);
           std::cout << "the user earased " << std::endl;
           return; // Exit the function after removing the user
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
    return ""; // Return empty string if operator not found
}

void Channel::removeOperator(const std::string& operatorName )
{
    // Iterate through the map to find the operator
    std::map<std::string, int>::iterator it;
    for (it = operators.begin(); it != operators.end(); ++it) {
        if (it->first == operatorName) {
            // Erase the operator from the map
            operators.erase(it);
            return; // Exit the function after removing the operator
        }
    }
}