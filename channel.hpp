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
    std::vector<std::string> users;
    // std::map<int, std::string> nicknames; // Replace unordered_map with map
    std::map<std::string, int> userFdMap; // Mapping of usernames to file descriptors
    std::map<std::string, int> invitedUsers;
    std::map<std::string, int>  operators;

public:
    // Constructors
    Channel() {}

    Channel(const std::string& name) : Channelname(name) {}

    // Destructor
    ~Channel() {}



    void setTopic(const std::string& newTopic) {
        topic = newTopic;
    }

    // Get topic function
    std::string getTopic() const {
        return topic;
    
    }

    void addClient(const std::string& client, int fd) {
        userFdMap[client] = fd;
    }

    void addClientinveted(const std::string& client, int fd) {
        invitedUsers[client] = fd;
    }

    // Add an operator to the channel
    void addOperator(const std::string& operatorName, int fd) {
    // Store the operator name and file descriptor in the map
    operators[operatorName] = fd;
    }

    int getUserFd(const std::string& username) const {
        std::map<std::string, int>::const_iterator it = userFdMap.find(username);
        if (it != userFdMap.end()) {
            return it->second;
        }
        return -1; // Return -1 if username not found
    }

    // Get all clients' usernames in the channel
    std::vector<std::string> getClients() const {
        std::vector<std::string> clients;
        std::map<std::string, int>::const_iterator it;
        for (it = userFdMap.begin(); it != userFdMap.end(); ++it) {
            clients.push_back(it->first);
        }
        return clients;
    }

    std::string getNickname(int fd) const {
        std::map<std::string, int>::const_iterator it;
        for (it = userFdMap.begin(); it != userFdMap.end(); ++it) {
        if (it->second == fd) {
            return it->first; // Return the nickname if the file descriptor matches
        }
        }
        return ""; // Return an empty string if the file descriptor is not found
    }


    bool isOperator(int fd) {
    // Iterate through the map of operators
    for (std::map<std::string, int>::iterator it = operators.begin(); it != operators.end(); ++it) {
        // Check if the file descriptor matches
        if (it->second == fd) {
            return true; // Found the file descriptor in the map
        }
    }
    return false; // File descriptor not found in the map
    }

    bool isInvited(std::string nickname) {
    // Iterate through the map of operators
    for (std::map<std::string, int>::iterator it = invitedUsers.begin(); it != invitedUsers.end(); ++it) {
        // Check if the file descriptor matches
        if (it->first == nickname) {
            return true; // Found the file descriptor in the map
        }
    }
    return false; // File descriptor not found in the map
    }

    

    int findUserFdForKickRegulars(const std::string& username) {
    // Iterate through the userFdMap to find the user
    std::map<std::string, int>::iterator it;
    for (it = userFdMap.begin(); it != userFdMap.end(); ++it) {
        if (it->first == username) {
            return it->second; // Return the file descriptor if the username matches
        }
    }
    return -1; // Return -1 if the user is not found
}


 void ejectUserfromusers(int fd) {
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

 void ejectUserfromivited(std::string nickname) {
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

std::string getOperatorNickname(int fd) const {
    std::map<std::string, int>::const_iterator it;
    for (it = operators.begin(); it != operators.end(); ++it) {
        if (it->second == fd) {
            return it->first;
        }
    }
    return ""; // Return empty string if operator not found
}

  void removeOperator(const std::string& operatorName )
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

    // Remove an operator from the channel
};



#endif