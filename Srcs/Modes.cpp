#include "Server.hpp"

void Server::processModeCmd(Client& client, const std::string& command, int fd) {
    std::string channelName, mode , nick;
    std::istringstream iss(command.substr(5));
    iss >> channelName >> mode >> nick;
    
    if (channelName[0] != '#') {
        client.clearCommand();
        return;
    }
    
    channelName = channelName.substr(1);
    channelName = trim(channelName);
    mode = trim(mode);
    size_t opt = mode.length() - 1;

    if (mode[opt] == 'o')
        handleOpPrivilege(nick, channelName, mode, fd);
    else if (mode[opt] == 't')
        handleTopicRestriction(nick, channelName, mode, fd);
    else if (mode[opt] == 'i')
        handleInviteOnly(nick, channelName, mode, fd);
    else if (mode[opt] == 'k')
        handleChannelKey(nick, channelName, mode, fd);
    else if (mode[opt] == 'l')
        handleChannelLimit(nick, channelName, mode, fd);
}

void Server::handleOpPrivilege(const std::string& nick, const std::string& channelName, const std::string& mode, int fd) {
    if (mode == "+o") {
        if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
            channels[channelName].addOperator(nick, channels[channelName].getUserFd(nick));
            abaaba = channels[channelName].getUserFd(nick);
            std::string modeChangeMessage = MODE_SET_MESSAGE(channelName, mode, fd, nick);
            send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
            smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
        }
        else {
            std::string errorMessage = ERROR_MESSAGE7(channelName, fd);
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
    else if (mode == "-o") {
        if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
            channels[channelName].removeOperator(nick);
            std::string modeChangeMessage = MODE_UNSET_MESSAGE(channelName, mode, fd, nick);
            send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
            smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
        }
        else {
            std::string errorMessage = ERROR_MESSAGE5();
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
}

void Server::handleTopicRestriction(const std::string& nick, const std::string& channelName, const std::string& mode, int fd) {
    if (mode == "-t") {
        if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
            std::string modeChangeMessage = TOPIC_CHANGE_MESSAGE(channelName, mode, fd);
            send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
            smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
            issettop = 1;
        }
        else {
            std::string errorMessage = ERROR_MESSAGE5();
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
    else if (mode == "+t") {
        if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
            std::string modeChangeMessage = TOPIC_CHANGE_MESSAGE(channelName, mode, fd);
            send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
            smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
            issettop = 0;
        }
        else {
            std::string errorMessage = ERROR_MESSAGE5();
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
}

void Server::handleInviteOnly(const std::string& nick, const std::string& channelName, const std::string& mode, int fd) {
    if (mode == "+i") {
        if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
            std::string modeChangeMessage = MODE_SET_MESSAGE(channelName, mode, fd, nick);
            send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
            smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
            isinvited = 1;
        }
        else if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd) == false) {
            std::string errorMessage = ERROR_MESSAGE5();
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
        
    }
    else if (mode == "-i") {
        if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
            std::string modeChangeMessage = MODE_UNSET_MESSAGE(channelName, mode, fd, nick);;
            send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
            smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
            isinvited = 0;
        }
        else {
            std::string errorMessage = ERROR_MESSAGE5();
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
}

void Server::handleChannelKey(std::string& nick, const std::string& channelName, const std::string& mode, int fd) {
    if (mode == "-k") {
        if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
            std::string modeChangeMessage = MODE_UNSET_MESSAGE(channelName, mode, fd, nick);
            send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
            smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
            itHasPass = 0;
        }
        else {
            std::string errorMessage = ERROR_MESSAGE5();
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
    else if (mode == "+k") {
        nick = trim(nick);
        channels[channelName].setPass(nick);
        if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
            std::string modeChangeMessage = MODE_SET_MESSAGE(channelName, mode, fd, nick);
            send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
            smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
            itHasPass = 1;
        }
        else {
            std::string errorMessage = ERROR_MESSAGE5();
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
}

void Server::handleChannelLimit(const std::string& nick, const std::string& channelName, const std::string& mode, int fd) {
    if (mode == "+l") {
        if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
            int limit = stringToInt(nick);
            channels[channelName].setlimitchannel(limit);
            std::string modeChangeMessage = MODE_SET_MESSAGE(channelName, mode, fd, nick);
            send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
            smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
            channelLimit = 1;
        }
        else {
            std::string errorMessage = ERROR_MESSAGE5();
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
    else if (mode == "-l") {
        if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
            std::string modeChangeMessage = MODE_UNSET_MESSAGE(channelName, mode, fd, nick);
            send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
            smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
            channelLimit = 0;
            limitchannelforincrement = 0;
        }
        else {
            std::string errorMessage = ERROR_MESSAGE5();
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
}
