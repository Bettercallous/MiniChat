#include "Server.hpp"

int opperatorfd = 0;
int issettop = 0;
int isinvited = 0;
int itHasPass = 0;
int channelLimit = 0;
int limitchannelforincrement = 0;
int abaaba = 0;

bool Server::_signal = false;

Server::Server() {}

Server::~Server() {}

std::string Server::getPassowrd() const {
    return _password;
}

void Server::receiveSignal(int signum) {
    _signal = true;
    (void)signum;
}

Client& Server::getClientByFd(int fd) {
    size_t i = 0;
    for (; i < _clients.size(); ++i) {
        if (_clients[i].getFd() == fd)
            break;
    }
    return _clients[i];
}

void Server::setNickname(int fd, const std::string& nickname) {nicknames[fd] = nickname;}

void Server::setUsernames(int fd, const std::string& username) {usernames[fd] = username;}

void Server::init() {
    signal(SIGINT, receiveSignal);
    signal(SIGQUIT, receiveSignal);

    createServerSocket();
    std::cout << GREEN << ">>> SERVER STARTED <<<" << RESET << std::endl;
    std::cout << CYAN <<"Waiting for connections..." << RESET << std::endl;
}

void Server::run() {
    while (!_signal) {
        int ret = poll(&_fds[0], _fds.size(), -1);
        if (ret == -1 && !_signal)
            throw std::runtime_error("Error: poll() failed");

        for (size_t i = 0; i < _fds.size(); ++i) {
            if (_fds[i].revents & POLLIN) {
                if (_fds[i].fd == _serverSocketFd)
                    handleClientConnection();
                else
                    handleClientData(_fds[i].fd);
            }
        }
    }
    closeFds();
}

void Server::createServerSocket() {
    _serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocketFd == -1)
        throw std::runtime_error("Error: failed to create socket");

    int optval = 1;
    if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        throw std::runtime_error("Error: setsockopt() failed");

    if (fcntl(_serverSocketFd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Error: fcntl() failed");

    bindServerSocket();

    if (listen(_serverSocketFd, SOMAXCONN) == -1)
        throw std::runtime_error("Error: listen() failed");

    addPollfd(_serverSocketFd);
}

void Server::bindServerSocket() {
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(_port);
    sa.sin_addr.s_addr = INADDR_ANY;
    if (bind(_serverSocketFd, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        throw std::runtime_error("Error: failed to bind socket");
    }
}

void Server::addPollfd(int fd) {
    struct pollfd newPollfd;
    newPollfd.fd = fd;
    newPollfd.events = POLLIN;
    newPollfd.revents = 0;
    _fds.push_back(newPollfd);
}

void Server::handleClientConnection() {
    struct sockaddr_in client_addr;
    socklen_t clientAddrSize = sizeof(sockaddr_in);
    int newFd = accept(_serverSocketFd, (struct sockaddr *)&client_addr, &clientAddrSize);
    if (newFd == -1)
        throw std::runtime_error("Error: accept() failed");

    if (fcntl(newFd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Error: fcntl() failed");

    std::string passwordRequest = "Please Enter The password Of This Server :\n";
    std::string art = MINICHAT;

    send(newFd, art.c_str(), art.length(), 0);
    send(newFd, passwordRequest.c_str(), passwordRequest.length(), 0);
    addPollfd(newFd);
    _clients.push_back(Client(newFd));
        
    std::cout << "Client <" << newFd << "> Connected" << std::endl;
}

void Server::sendJoinMsg(const std::string& nickname, const std::string& channelName, int fd) {
    std::string operators = channels[channelName].getOperatorNickname(opperatorfd);
    std::string operators1 = channels[channelName].getOperatorNickname(abaaba);

    std::string topic = channels[channelName].getTopic();
    std::string creationTimeMessage = constructJoinedTimeMessage(channelName);
    std::string joinMessage = JOIN_MESSAGE(nickname, channelName);
    std::string topicMessage = TOPIC_MESSAGE(nickname, channelName, topic);

    send(fd, joinMessage.c_str(), joinMessage.length(), 0);
    send(fd, topicMessage.c_str(), topicMessage.length(), 0);

    std::string namesMessage = NAMES_MESSAGE2(nickname, channelName);
    const std::vector<std::string>& clients = channels[channelName].getClients();
    for (size_t i = 0; i < clients.size(); ++i) {
        const std::string& user = clients[i];
        if (user == operators || user == operators1) {
            namesMessage += "@" + user;
        } else {
            namesMessage += user;
        }
        if (i < clients.size() - 1)
            namesMessage += " ";
    }
    namesMessage += "\n";
    std::string endOfNamesMessage = END_OF_NAMES_MESSAGE(nickname, channelName);
    std::string channelMessage = CHANNEL_MESSAGE(channelName, creationTimeMessage);

    send(fd, namesMessage.c_str(), namesMessage.length(), 0);
    send(fd, endOfNamesMessage.c_str(), endOfNamesMessage.length(), 0);
    send(fd, channelMessage.c_str(), channelMessage.length(), 0);

    smallbroadcastMessageforjoin(nickname, channelName);
}

// handling private msg between users only 
void Server::handlePrivateMessage(int senderFd, const std::string& recipient, const std::string& message) {
    int recipientFd = findUserFd1(recipient);
    if (recipientFd != -1) {
        std::string privateMessage = PRIVATE_MESSAGE(senderFd, recipient, message);
        send(recipientFd, privateMessage.c_str(), privateMessage.length(), 0);
    } else {
        std::string errorMessage = ERROR_MESSAGE(senderFd, recipient);
        send(senderFd, errorMessage.c_str(), errorMessage.length(), 0);
    }
}

void Server::handleInvitation(int senderFd, const std::string& recipient, std::string channelName) {
    int recipientFd = findUserFd1(recipient);

    if (recipientFd != -1) {
        std::string inviteMessage = INVITE_MESSAGE(senderFd, recipient, channelName);
        send(recipientFd, inviteMessage.c_str(), inviteMessage.length(), 0);
    } else {
        std::string errorMessage = ERROR_MESSAGE(senderFd, recipient);
        send(senderFd, errorMessage.c_str(), errorMessage.length(), 0);
    }
}

void Server::handleClientData(int fd)
{
    Client& client = getClientByFd(fd); 
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRead = recv(fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        client.appendCommand(buffer);
        size_t newlinePos = client.getCommand().find_first_of("\r\n");
        if (newlinePos != std::string::npos) {
            std::string command = client.getCommand().substr(0, newlinePos);
            client.setCommand(command);
            std::cout << "Received data from client " << fd << ": " << command << std::endl;
            int auth = client.getAuthentication();

            if ((startsWith(command, "PASS ") || startsWith(command, "pass ")) && auth == 0)
                processPassword(client, command, fd);
            else if ((startsWith(command, "NICK ") || startsWith(command, "nick ")) && auth == 1)
                processNickCmd(client, command, fd);
            else if ((startsWith(command, "USER ") || startsWith(command, "user ")) && auth == 2)
                processUserCmd(client, command, fd);
            else if (startsWith(command, "JOIN ") || startsWith(command, "join "))
                processJoinCmd(client, command, fd);
            else if (startsWith(command, "PRIVMSG ") || startsWith(command, "privmsg "))
                processPrivmsgCmd(client, command, fd);
            else if (startsWith(command, "KICK ") || startsWith(command, "kick "))
                processKickCmd(client, command, fd);
            else if (startsWith(command, "TOPIC ") || startsWith(command, "topic "))
                processTopicCmd(client, command, fd);
            else if (startsWith(command, "INVITE ") || startsWith(command, "invite "))
                processInviteCmd(client, command, fd);
            else if (startsWith(command, "BOT ") || startsWith(command, "bot "))
                processBotCmd(client, command, fd);
            else if (startsWith(command, "MODE ") || startsWith(command, "mode "))
                processModeCmd(client, command, fd);
            else if (startsWith(command, "QUIT") || startsWith(command, "quit"))
                processQuitCmd(fd);
            else if (startsWith(command, "PING"))
                ping(command, fd);
            
            client.clearCommand();
        }
    }

    else if (bytesRead == 0) {
        std::cout << "Client <" << fd << "> Disconnected" << std::endl;
        cleanChannel(fd);
        clientCleanup(fd);
    } else if (bytesRead == -1) {
        std::cerr << "Error reading data from client <" << fd << ">" << std::endl;
        cleanChannel(fd);
        clientCleanup(fd);
    }
}
