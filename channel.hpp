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


class Channel {
private:
    std::string Channelname;
    std::string topic;
    std::string key;
    std::vector<std::string> users;
    // std::map<int, std::string> nicknames; // Replace unordered_map with map
    std::map<std::string, int> userFdMap; // Mapping of usernames to file descriptors
    std::vector<std::string> invitedUsers;
    std::vector<std::string> operators;

public:
    // Constructors
    Channel(const std::string& name) : Channelname(name) {}

    // Destructor
    ~Channel() {}

    // Add a client to the channel


    

    void addClient(const std::string& client, int fd) {
        userFdMap[client] = fd;
    }

    void addClient(const std::string& client) {
        users.push_back(client);
    }

    // Remove a client from the channel
    void removeClient(const std::string& nickname) {
        // Implement removal logic
        // Iterate through clients vector, find the client by nickname, and remove it
    }

    // Add an invited user to the channel
    void inviteUser(const std::string& user) {
        invitedUsers.push_back(user);
    }

    // Remove an invited user from the channel
    void uninviteUser(const std::string& user) {
        // Implement removal logic
        // Iterate through invitedUsers vector, find the user, and remove it
    }

    // Add an operator to the channel
    void addOperator(const std::string& operatorName) {
        operators.push_back(operatorName);
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

    // Remove an operator from the channel
};



#endif