#include "Server.hpp"

void Server::ping(const std::string& command, int fd) {
    std::istringstream iss(command);
    std::string serverHostname = command.substr(5);
    std::string pongMessage = "PONG " + serverHostname + "\r\n";
    send(fd, pongMessage.c_str(), pongMessage.length(), 0);
    std::cout << "PONG" << std::endl;
}

void Server::processJoinCmd(Client& client, const std::string& command, int fd)
{
    std::string nick;
    for (size_t i = 0; i < _clients.size(); ++i) {
        if (_clients[i].getFd() == fd) {
            nick = _clients[i].getNick();
            break;
        }
    }
    std::string channelName, pass ;
    std::istringstream iss(command.substr(5));
    iss >> channelName ;
    if (channelName[0] != '#')
    {
        std::string what = " :Error: Channel start with #\r\n";
        std::string errorMessage = ERROR_MESSAGE2(nick);
            send(fd, errorMessage.c_str(), errorMessage.length(), 0);
        client.clearCommand();
        return;
    }
    channelName = channelName.substr(1);
    channelName = trim(channelName);
    std::getline(iss, pass);
    pass = trim(pass);

    // Check if the channel already exists
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it != channels.end()) 
    {
        // Channel already exists
        std::string what = " :Error: CHANNEL limit\r\n";
        if ((isinvited == 1 && channels[channelName].isInvited(nick)) || channels[channelName].isOperator(fd))
        {
            // User is invited, create the channel
            int check = channels[channelName].getChannelLimit();
            if (limitchannelforincrement < check || channelLimit == 0)
                createChannel(channelName, nick, fd);
            else {
                std::string errorMessage = ERROR_MESSAGE2(nick);
                send(fd, errorMessage.c_str(), errorMessage.length(), 0);
            }
        } 
        else if (isinvited == 0) {
            if (itHasPass == 1 && channels[channelName].getPass() == pass) {
                int check = channels[channelName].getChannelLimit();
                if (limitchannelforincrement < check || channelLimit == 0)
                    createChannel(channelName, nick, fd);
                else {
                    std::string errorMessage = ERROR_MESSAGE2(nick);
                    send(fd, errorMessage.c_str(), errorMessage.length(), 0);
                }
            }
            else if (itHasPass == 0) {
                int check = channels[channelName].getChannelLimit();
                if (limitchannelforincrement < check || channelLimit == 0)
                    createChannel(channelName, nick, fd);
                else {
                    std::string errorMessage = ERROR_MESSAGE2(nick);
                    send(fd, errorMessage.c_str(), errorMessage.length(), 0);
                }
            }
            else if (itHasPass == 1 && channels[channelName].getPass() != pass) {
                what = " :Error: you need a password for this channel\r\n";
                std::string errorMessage = ERROR_MESSAGE2(nick);
                send(fd, errorMessage.c_str(), errorMessage.length(), 0);
            }

        }
        else {
            // User is not invited, send error message
            what = " :Error: you are not invited\r\n";
            std::string errorMessage = ERROR_MESSAGE2(nick);
            send(fd, errorMessage.c_str(), errorMessage.length(), 0);
        }
    } 
    else 
        createChannel(channelName, nick, fd);
}

void Server::processPrivmsgCmd(Client& client, const std::string& command, int fd)
{
    std::istringstream iss(command);
    std::string cmd, recipient, message;
    std::string niiick;

    iss >> cmd >> recipient;
    recipient = trim(recipient);
    std::getline(iss, message);
    
    if (iss.fail()) {
        std::string errorMessage = "Error: You Just missing an argument(5)\n";
        send(fd, errorMessage.c_str(), errorMessage.length(), 0);
        client.clearCommand();
        return;
    }
    
    message = trim(message);
    message = message.substr(1);
    if (recipient[0] == '#') {
        recipient = recipient.substr(1);
        for (size_t i = 0; i < _clients.size(); ++i) {
            if (_clients[i].getFd() == fd) {
                niiick =  _clients[i].getNick();
                break;
            }
        }
        broadcastMessage(recipient, niiick, message);
    }
    else {
        handlePrivateMessage(fd, recipient, message);
    }
}

void Server::processQuitCmd(int fd) {
    cleanChannel(fd);
    clientCleanup(fd);
    std::cout << "Client <" << fd << "> Disconnected" << std::endl;
}

void Server::processKickCmd(Client& client, const std::string& command, int fd)
{
    std::string channelName, userToKick, reason;
    std::istringstream iss(command.substr(5));
    iss >> channelName >> userToKick;  

    if (iss.fail()) {
        std::string errorMessage = "Error: You Just missing an argument(4)\n";
        send(fd, errorMessage.c_str(), errorMessage.length(), 0);
        client.clearCommand();
        return;
    }

    std::getline(iss, reason);
    channelName = channelName.substr(1);
    channelName = trim(channelName);
    userToKick = trim(userToKick);
    reason = trim(reason);

    std::string sender = channels[channelName].getNickname(fd);
    if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
        int userFd = channels[channelName].findUserFdForKickRegulars(userToKick);
        if (userFd != -1) {
            channels[channelName].ejectUserfromusers(userFd);
            channels[channelName].ejectUserfromivited(userToKick);
            std::string kickMessage = KICK_MESSAGE2(channelName, fd, userToKick, reason);
            smallBroadcastMsgForKick(sender, channelName, userToKick, reason);
            send(fd, kickMessage.c_str(), kickMessage.length(), 0);
            send(userFd , kickMessage.c_str(), kickMessage.length(), 0);
        } 
        else {
            std::string errorMessage = ERROR_MESSAGE3(sender, channelName, userToKick);
            send(fd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
    else {
        std::string errorMessage = ERROR_MESSAGE4(sender, channelName, userToKick);
        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
    }
}

void Server::processTopicCmd(Client& client, const std::string& command, int fd)
{
    std::string channelName, topic;
    std::istringstream iss(command.substr(6));
    iss >> channelName;
    std::getline(iss, topic);
    
    if (iss.fail()) {
        std::string errorMessage = "Error: You Just missing an argument(3)\n";
        send(fd, errorMessage.c_str(), errorMessage.length(), 0);
        client.clearCommand();
        return;
    }
    
    channelName = channelName.substr(1);
    channelName = trim(channelName);
    topic = trim(topic);
    topic = topic.substr(1);
    
    std::string sender = channels[channelName].getNickname(fd);
    if ((channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) || issettop == 1) {
        channels[channelName].setTopic(topic);
        std::string topicMessage = TOPIC_MESSAGE2(sender, channelName, topic);
        send(fd, topicMessage.c_str(), topicMessage.size(), 0);
        smallbroadcastMessageforTopic(sender, channelName, topic);
    }
    else {
        std::string errorMessage = ERROR_MESSAGE6(sender, channelName);
        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
    }
}

void Server::processInviteCmd(Client& client, const std::string& command, int fd) {
    std::string channelName, nickname;
    std::istringstream iss(command.substr(7));
    iss >> nickname >> channelName;
    
    if (iss.fail()) {
        std::string errorMessage = "Error: You Just missing an argument(2)\n";
        send(fd, errorMessage.c_str(), errorMessage.length(), 0);
        client.clearCommand();
        return;
    }

    channelName = trim(channelName);
    nickname = trim(nickname);
    channelName = channelName.substr(1);
    
    if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
        channels[channelName].addClientinveted(nickname, fd);
        handleInvitation(fd, nickname, channelName);
    }
    else {
        std::string errorMessage = ERROR_MESSAGE7(channelName, fd);
        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
    }
}

void Server::processBotCmd(Client& client, const std::string& command, int fd) {
    std::string start, end, guessed;
    std::istringstream iss(command.substr(4));
    iss >> start >> end >> guessed;
    std::string reme;
    
    if (iss.fail() || iss >> reme)
    {
        std::string errorMessage = "Error: Command take 3 parameters\n";
        send(fd, errorMessage.c_str(), errorMessage.length(), 0);
        client.clearCommand();
        return;
    }

    start = trim(start);
    end = trim(end);
    guessed = trim(guessed);

    if (stringToInt(start) < stringToInt(end) && stringToInt(guessed) >= stringToInt(start) && stringToInt(guessed) <= stringToInt(end)) {
        int r = randomInRange(stringToInt(start), stringToInt(end));
        std::string random = intToString(r);
        if (random == guessed) {
            std::string congratsMsg = CONGRATS_MSG(guessed);
            send(fd, congratsMsg.c_str(), congratsMsg.size(), 0);
        }
        else {
            std::string guessAgain = GUESS_AGAIN(random);
            send(fd, guessAgain.c_str(), guessAgain.size(), 0);
        }
    }
    else {
        std::string errorMessage = GUESS_ERROR();
        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
    }
}