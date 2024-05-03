#include "Server.hpp"

// brodcasting msg to all nicks in the channel 
void Server::broadcastMessage(const std::string& channel, const std::string& senderNickname, const std::string& msg) {
    std::map<std::string, Channel>::iterator it = channels.find(channel);
    if (it == channels.end()) {
        std::cerr << "Channel " << channel << " does not exist" << std::endl;
        return;
    }

    if (channels[channel].findUserFdForKickRegulars(senderNickname) == -1) {
        return;
    }

    std::string message = ":" + senderNickname + " PRIVMSG #" + channel + " :" + msg + "\r\n";
    const std::vector<std::string>& clients = it->second.getClients();

    for (size_t i = 0; i < clients.size(); ++i) {
        const std::string& client = clients[i];

        if (client == senderNickname) {
            continue;
        }

        int recipientFd = it->second.getUserFd(client);

        if (recipientFd != -1) {
            std::cout << message << std::endl;
            send(recipientFd, message.c_str(), message.size(), 0);
        } else {
            std::cerr << "Client " << client << " not found" << std::endl;
        }
    }
}

void Server::smallBroadcastMsgForKick(std::string sender , const std::string& channelname, const std::string& usertokick, const std::string& reason) {
    std::map<std::string, Channel>::iterator it = channels.find(channelname);
    if (it == channels.end()) {
        std::cerr << "Channel " << channelname << " does not exist" << std::endl;
        return;
    }
    std::string kickMessage = KICK_MESSAGE(sender, channelname, usertokick, reason);

    const std::vector<std::string>& clients = it->second.getClients();

    for (size_t i = 0; i < clients.size(); ++i) {
        const std::string& client = clients[i];

        if (client == sender) {
            continue;
        }
        int recipientFd = it->second.getUserFd(client);
        if (recipientFd != -1) {
            send(recipientFd, kickMessage.c_str(), kickMessage.size(), 0);
        } else {
            std::cerr << "Client " << client << " not found" << std::endl;
        }
    }
}

void Server::smallbroadcastMessageforjoin(std::string nickname , const std::string& channelName) {
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end()) {
        std::cerr << "Channel " << channelName << " does not exist" << std::endl;
        return;
    }
    std::string joinMessage = JOIN_MESSAGE(nickname, channelName);
    const std::vector<std::string>& clients = it->second.getClients();

    for (size_t i = 0; i < clients.size(); ++i) {
        const std::string& client = clients[i];

        if (client == nickname) {
            continue;
        }
        int recipientFd = it->second.getUserFd(client);
        if (recipientFd != -1) {
            send(recipientFd, joinMessage.c_str(), joinMessage.size(), 0);
        } else {
            std::cerr << "Client " << client << " not found" << std::endl;
        }
    }
}

void Server::smallbroadcastMessageforTopic(std::string nicknamesender, const std::string& channelname, std::string topic) {
    std::map<std::string, Channel>::iterator it = channels.find(channelname);
    if (it == channels.end()) {
        std::cerr << "Channel " << channelname << " does not exist" << std::endl;
        return;
    }

    std::string topicMessage = TOPIC_MESSAGE2(nicknamesender, channelname, topic);
    const std::vector<std::string>& clients = it->second.getClients();

    for (size_t i = 0; i < clients.size(); ++i) {
        const std::string& client = clients[i];

        if (client == nicknamesender) {
            continue;
        }
        int recipientFd = it->second.getUserFd(client);
        if (recipientFd != -1) {
            send(recipientFd, topicMessage.c_str(), topicMessage.size(), 0);
        } else {
            std::cerr << "Client " << client << " not found" << std::endl;
        }
    }
}

void Server::smallbroadcastMOOD(std::string sender, const std::string& channelname, std::string mode, std::string receiver) {

    std::map<std::string, Channel>::iterator it = channels.find(channelname);
    if (it == channels.end()) {
        std::cerr << "Channel " << channelname << " does not exist" << std::endl;
        return;
    }

    std::string modeChangeMessage = MODE_CHANGE_MESSAGE(channelname, mode, sender, receiver);
    
    int senderFd = it->second.getUserFd(sender);
    const std::vector<std::string>& clients = it->second.getClients();
    for (size_t i = 0; i < clients.size(); ++i) {
        const std::string& client = clients[i];

        int recipientFd = it->second.getUserFd(client);
        if (recipientFd != -1 && recipientFd != senderFd) {
            send(recipientFd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
        } else {
            std::cerr << "Client " << client << " not found or is the sender" << std::endl;
        }
    }
}