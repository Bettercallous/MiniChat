#include "Server.hpp"
int a = 0;
int opperatorfd = 0;
int issettop = 0;
int isinveted = 0;
int itHasPass = 0;


bool Server::_signal = false;

Server::Server() {}

Server::~Server() {}

// void Server::setPassword(const std::string& password) {
//     _password = password;
// }

std::string Server::getPassowrd() const {
    return _password;
}

void Server::parseArgs(int ac, char **av) {
    if (ac != 3)
        throw std::runtime_error("Usage: ./ircserv <port> <password>");
    std::string port(av[1]);
    std::string pwd(av[2]);
    if (port.empty() || port.find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("Error: Invalid arguments");

    long _port = atol(av[1]);
    if (!(_port >= 1 && _port <= 65535))
        throw std::runtime_error("Error: Invalid arguments");

    if (pwd.empty())
        throw std::runtime_error("Error: Invalid arguments");

    this->_port = _port;
    this->_password = pwd;
}

void Server::receiveSignal(int signum) {
    _signal = true;
    (void)signum;
}

void Server::init() {
    signal(SIGINT, receiveSignal);
    signal(SIGQUIT, receiveSignal);

    createServerSocket();
    std::cout << ">>> SERVER STARTED <<<" << std::endl;
    std::cout << "Waiting for connections..." << std::endl;
}

void Server::run() {
    while (!_signal) {
        int ret = poll(&_fds[0], _fds.size(), -1);
        if (ret == -1)
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

    addPollfd(_serverSocketFd, POLLIN, 0);
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

void Server::addPollfd(int fd, short events, short revents) {
    struct pollfd newPollfd;
    newPollfd.fd = fd;
    newPollfd.events = events;
    newPollfd.revents = revents;
    _fds.push_back(newPollfd);
}

void Server::handleClientConnection() {
    for (size_t i = 0; i < _fds.size(); ++i) {
        if (_fds[i].fd == _serverSocketFd && (_fds[i].revents & POLLIN)) {
            struct sockaddr_in client_addr;
            socklen_t clientAddrSize = sizeof(sockaddr_in);
            int newFd = accept(_serverSocketFd, (struct sockaddr *)&client_addr, &clientAddrSize);
            if (newFd == -1) {
                throw std::runtime_error("Error: accept() failed");
            }
            std::string passwordRequest = "Please Enter The password Of This Server :\n";



std::string art =
    " ________                    ______      \n"
    "/        |                  /      \\     \n"
    "$$$$$$$/______    ______  /$$$$$$  |       \n"
    "   $$ | /      \\  /      \\ $$ | _$$/     \n"
    "   $$ |/$$$$$$  |/$$$$$$  |$$ |/    |      \n"
    "   $$ |$$ |  $$ |$$ |  $$ |$$ |$$$$ |       \n"
    "   $$ |$$ \\__$$ |$$ |__$$ |$$ \\__$$ |    \n"
    "   $$ |$$    $$/ $$    $$/ $$    $$/    \n"
    "   $$/  $$$$$$/  $$$$$$$/   $$$$$$/  \n"
    "                 $$ |                     \n"
    "                 $$ |                     \n"
    "                 $$/                      \n";

            send(newFd, art.c_str(), art.length(), 0);

        
            send(newFd, passwordRequest.c_str(), passwordRequest.length(), 0);



            addPollfd(newFd, POLLIN, 0);
            _clients.push_back(Client(newFd, inet_ntoa((client_addr.sin_addr))));
            

            std::cout << "Client <" << newFd << "> Connected" << std::endl;
        }
    }
}


// FUNCTIONS OF TOP GGGGGG.. START FROM HEEEREEE 

//triiiiiiiiiiiimmmmmm
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");

    return str.substr(first, last - first + 1);
}

std::string Server::formatCreationTime() {
    std::time_t currentTime = std::time(NULL);
    std::tm* localTime = std::localtime(&currentTime);

    char buffer[80]; 
    std::strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", localTime);
    return std::string(buffer);
}


std::string Server::constructCreationTimeMessage(const std::string& channelName) {
    std::stringstream ss;
    ss << "Channel #" << channelName << " created " << formatCreationTime();
    return ss.str();
}
std::string Server::constructJoinedTimeMessage(const std::string& channelName) {
    std::stringstream ss;
    ss << "Channel #" << channelName << " Joined " << formatCreationTime();
    return ss.str();
}


// here i create the channel and add the nicks of the users with some checks
void Server::createChannel(const std::string& channelName, const std::string& nickname, int fd) {
    // Check if the channel already exists
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end()) {
        isinveted = 0;
        
        // Channel doesn't exist, so create it and add the user
        Channel newChannel(channelName);
        newChannel.addClient(nickname, fd);
        newChannel.addOperator(nickname, fd);
        opperatorfd = fd;
        std::string creationTimeMessage = constructCreationTimeMessage(channelName);

        std::string joinMessage = ":" + nickname + " JOIN #" + channelName + "\n";
        send(fd, joinMessage.c_str(), joinMessage.length(), 0);
        std::string modeMessage = ":irc.TopG MODE #" + channelName + " +nt\n";
        send(fd, modeMessage.c_str(), modeMessage.length(), 0);
        std::string namesMessage = ":irc.TopG 353 " + nickname + " = #" + channelName + " :@" + nickname + "\n";
        send(fd, namesMessage.c_str(), namesMessage.length(), 0);
        std::string endOfNamesMessage = ":irc.TopG 366 " + nickname + " #" + channelName + " :End of /NAMES list.\n";
        send(fd, endOfNamesMessage.c_str(), endOfNamesMessage.length(), 0);
        std::string channelMessage = ":irc.TopG 354 " + channelName + " " + creationTimeMessage + "\n";
        send(fd, channelMessage.c_str(), channelMessage.length(), 0);

        // Insert the new channel into the map
        channels.insert(std::make_pair(channelName, newChannel));

    }

    else
    {
    // Channel already exists, just add the user to it
        it->second.addClient(nickname, fd);
        std::string operators = channels[channelName].getOperatorNickname(opperatorfd);
        std::string creationTimeMessage = constructJoinedTimeMessage(channelName);


        // Send JOIN message to the client
        std::string joinMessage = ":" + nickname + " JOIN #" + channelName + "\n";
        send(fd, joinMessage.c_str(), joinMessage.length(), 0);
        std::cout << "this is the topi and he good :  "<< channels[channelName].getTopic()  << std::endl;
        std::string topicMessage = ":irc.TopG 332 " + nickname + " #" + channelName + " :" + channels[channelName].getTopic() + " https://irc.com\n";
        send(fd, topicMessage.c_str(), topicMessage.length(), 0);
        std::string namesMessage = ":irc.TopG 353 " + nickname + " @ #" + channelName + " :";
        const std::vector<std::string>& clients = channels[channelName].getClients();


        for (size_t i = 0; i < clients.size(); ++i) {
            const std::string& user = clients[i];
            if (user == operators) {
                namesMessage += "@" + user;
            } else {
                namesMessage += user;
            }

            if (i < clients.size() - 1) {
                namesMessage += " ";
            }
        }
        namesMessage += "\n";
        send(fd, namesMessage.c_str(), namesMessage.length(), 0);
        std::string endOfNamesMessage = ":irc.example.com 366 " + nickname + " #" + channelName + " :End of /NAMES list.\n";
        send(fd, endOfNamesMessage.c_str(), endOfNamesMessage.length(), 0);
        std::string channelMessage = ":irc.example.com 354 " + channelName + " " + creationTimeMessage + "\n";
        send(fd, channelMessage.c_str(), channelMessage.length(), 0);

        smallbroadcastMessageforjoin(nickname, channelName);
    }
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.substr(0, prefix.length()) == prefix;
}

void Server::setNickname(int fd, const std::string& nickname) {
        nicknames[fd] = nickname;
    }

void Server::setUsernames(int fd, const std::string& username) {
        usernames[fd] = username; 
    }

//handling privet msg between users only 
void Server::handlePrivateMessage(int senderFd, const std::string& recipient, const std::string& message) {
    int recipientFd = findUserFd1(recipient);
    if (recipientFd != -1) {
        std::string privateMessage = ":" + nicknames[senderFd] + " PRIVMSG " + recipient + " :" + message + "\r\n";
        send(recipientFd, privateMessage.c_str(), privateMessage.length(), 0);
    } else {
        std::string errorMessage = ":server.host NOTICE " + nicknames[senderFd] + " :Error: User '" + recipient + "' not found or offline\r\n";
        send(senderFd, errorMessage.c_str(), errorMessage.length(), 0);
    }
}

void Server::handleInvitation(int senderFd, const std::string& recipient, std::string channelName) {
    int recipientFd = findUserFd1(recipient);

    if (recipientFd != -1) {
        std::string inviteMessage = ":" + nicknames[senderFd] + " INVITE " + recipient + " :#" + channelName + "\r\n";
        send(recipientFd, inviteMessage.c_str(), inviteMessage.length(), 0);
    } else {
        std::string errorMessage = ":server.host NOTICE " + nicknames[senderFd] + " :Error: User '" + recipient + "' not found or offline\r\n";
        send(senderFd, errorMessage.c_str(), errorMessage.length(), 0);
    }
}

//this find is for finding nickname of the users i need to brodcasting to 
int Server::findUserFd1(const std::string& username) {
    std::map<int, std::string>::iterator it;
    for (it = nicknames.begin(); it != nicknames.end(); ++it) {
        if (it->second == username) {
            return it->first; 
        }
    }
    return -1;
}

bool Server::dontputthesamenick(const std::string& nickname) {
    std::map<int, std::string>::iterator it;
    for (it = nicknames.begin(); it != nicknames.end(); ++it) {
        if (it->second == nickname) {
            return true; 
        }
    }
    return false; 
}

bool Server::dontputthesameusername(const std::string& username) {
    std::map<int, std::string>::iterator it;
    for (it = usernames.begin(); it != usernames.end(); ++it) {
        if (it->second == username) {
            return true; 
        }
    }
    return false; 
}

int Server::findUserFdforkickregulars(const std::string& username) {
    std::map<int, std::string>::iterator it;
    for (it = usernamesregulars.begin(); it != usernamesregulars.end(); ++it) {
        if (it->second == username) {
            return it->first;
        }
    }
    return -1;
}


// brodcasting msg to all nicks in the  channel 
void Server::broadcastMessage(const std::string& channel, const std::string& senderNickname, const std::string& msg, int fd) {
    std::map<std::string, Channel>::iterator it = channels.find(channel);
    if (it == channels.end()) {
        std::cerr << "Channel " << channel << " does not exist" << std::endl;
        return;
    }

    if (channels[channel].findUserFdForKickRegulars(senderNickname) == -1)
    {
        return;
    }
    std::string message = ":" + senderNickname + " PRIVMSG #" + channel + " :" + msg + "\r\n";

    const std::vector<std::string>& clients = it->second.getClients();

    for (size_t i = 0; i < clients.size(); ++i) {
        const std::string& client = clients[i];

        std::cout << "this is the client name : "<< client <<  std::endl;
        std::cout << "this is the nickname name : " << senderNickname << std::endl;

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



void Server::smallbroadcastMessagefortheckick(std::string nicknamesender , const std::string& channelname, const std::string& usertokick, const std::string& reason) {
    std::map<std::string, Channel>::iterator it = channels.find(channelname);
    if (it == channels.end()) {
        std::cerr << "Channel " << channelname << " does not exist" << std::endl;
        return;
    }
    std::string kickMessage = ":" + nicknamesender + " KICK #" + channelname + " " + usertokick + " :" + reason + "\n";

    const std::vector<std::string>& clients = it->second.getClients();

    for (size_t i = 0; i < clients.size(); ++i) {
        const std::string& client = clients[i];

        if (client == nicknamesender) {
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

void Server::smallbroadcastMessageforjoin(std::string nicknamesender , const std::string& channelname) {
    std::map<std::string, Channel>::iterator it = channels.find(channelname);
    if (it == channels.end()) {
        std::cerr << "Channel " << channelname << " does not exist" << std::endl;
        return;
    }
    std::string joinMessage = ":" + nicknamesender + " JOIN #" + channelname + "\n";

    const std::vector<std::string>& clients = it->second.getClients();

    for (size_t i = 0; i < clients.size(); ++i) {
        const std::string& client = clients[i];

        if (client == nicknamesender) {
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

    std::string topicMessage = ":" + nicknamesender + " TOPIC #" + channelname + " :" + topic + "\n";

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


void Server::smallbroadcastMOOD(std::string nicknamesender, const std::string& channelname, std::string mode, std::string receiver) {

    std::map<std::string, Channel>::iterator it = channels.find(channelname);
    if (it == channels.end()) {
        std::cerr << "Channel " << channelname << " does not exist" << std::endl;
        return;
    }

    std::string modeChangeMessage;
    if (mode == "+t") {
        modeChangeMessage = ":server.host MODE #" + channelname + " +t by " + nicknamesender + "\n";
    } else if(mode == "-t") {
        modeChangeMessage = ":server.host MODE #" + channelname + " -t by " + nicknamesender + "\n";
    }
    if (mode == "+o"){
        modeChangeMessage = ":server.host MODE #" + channelname + " " + mode + " by " + nicknamesender + " and set " + receiver + " as operator\n";
    }
    else if (mode == "-o"){
        modeChangeMessage = ":server.host MODE #" + channelname + " " + mode + " by " + nicknamesender + " and unset " + receiver + " as operator\n";
    }
    else if (mode == "+i"){
        modeChangeMessage = ":server.host MODE #" + channelname + " +i by " + nicknamesender + "\n";
    }
    else if (mode == "-i"){
        modeChangeMessage = ":server.host MODE #" + channelname + " -i by " + nicknamesender + "\n";
    }

    int senderFd = it->second.getUserFd(nicknamesender);

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


void Server::kickUser(int fd) {
    close(fd);
    std::map<int, std::string>::iterator it = usernamesregulars.find(fd);
    if (it != usernamesregulars.end()) {
        usernamesregulars.erase(it);
    }
}



// ........AND FINISH HERE ......//////


void Server::handleClientData(int fd) {
    std::string command;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRead;
    while ((bytesRead = recv(fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        bool foundEof = false;
        for (ssize_t i = 0; i < bytesRead; ++i) {
            if (buffer[i] == '\n') {
                foundEof = true;
                break;
            }
        }

        if (!foundEof) {
            buffer[bytesRead] = '\0';
            command += buffer;
        } else {
            command.append(buffer, bytesRead - 1);
            std::cout << "Received data from client " << fd << ": " << command << std::endl;

//******************* FROM THERE IM STARTING TOP GGG ************  .
            if (startsWith(command, "pass"))
            {
                std::string cmd, password;
                std::istringstream iss(command);
                iss >> cmd >> password;
                password = trim(password); // Remove leading/trailing whitespace
                std::string  passwordoftheserver = getPassowrd();
                if (passwordoftheserver != password)
                {
                    std::string errorMessage = "Error: Incorrect Password\n";
                    send(fd, errorMessage.c_str(), errorMessage.length(), 0);
                }
                else {
                    std::string confirmation = "Please Enter Your Nickname : \n";
                    send(fd, confirmation.c_str(), confirmation.length(), 0);
                    a = 1;
                }
            }
            else if (startsWith(command, "PING"))
            {
                std::istringstream iss(command);
                std::string serverHostname = command.substr(5); // Skip the "PING " prefix
                std::string pongMessage = "PONG " + serverHostname + "\r\n";
                send(fd, pongMessage.c_str(), pongMessage.length(), 0);
                std::cout << "ping was sent" << std::endl;
            }

            else if (startsWith(command, "nick") && a == 1) 
            {
                std::string cmd, nick;
                std::istringstream iss(command);
                iss >> cmd >> nick;
                nick = trim(nick);
                if (dontputthesamenick(nick) == true)
                {
                    std::string confirmation = "Please Use a Different Nickname : \n";
                    send(fd, confirmation.c_str(), confirmation.length(), 0);
                }
                else
                {
                    setNickname(fd, nick);
                    for (size_t i = 0; i < _clients.size(); ++i)
                    {
                        if (_clients[i].getFd() == fd)
                        {
                            _clients[i].setNick(nick);
                            std::cout << "Password set for client " << fd << ": " << nick << std::endl;
                            break;
                        }
                    }
                    std::string confirmation = "Please Enter Your Username : \n";
                    send(fd, confirmation.c_str(), confirmation.length(), 0);

                    a = 2;

                }
            } 
            
            else if (startsWith(command, "user") && a == 2) 
            {

                std::istringstream iss(command);
                std::string cmd, username, dontworry, dontworry1, realname, nickname;
                iss >> cmd >> username >> dontworry >> dontworry1 >> realname;
                username = trim(username);
                dontworry = trim(dontworry);
                dontworry1 = trim(dontworry1);
                realname = trim(realname);

                if (dontputthesameusername(username) == true)
                {
                    std::string confirmation = "Please Use a Different Nickname : \n";
                    send(fd, confirmation.c_str(), confirmation.length(), 0);
                }
                else
                {
                    setUsernames(fd, username);

                    for (size_t i = 0; i < _clients.size(); ++i)
                    {
                        if (_clients[i].getFd() == fd)
                        {
                            _clients[i].setUser(username);
                            _clients[i].setName(realname);
                            nickname = _clients[i].getNick();
                            break;
                        }
                    }
                    a = 0;

                    std::string one = ":irc.l9oroch 001 " + nickname + " :Welcome to the TopG Network, " + nickname + '\n';
                    std::string two = ":irc.l9oroch 002 " + nickname + " :Your host is TopG, running version 4.5" + '\n';
                    std::string tre = ":irc.l9oroch 003 " + nickname + " :This server was created " + formatCreationTime() + '\n';
                    std::string foor = ":irc.l9oroch 004 " + nickname + " TopG TopG(enterprise)-2.3(12)-netty(5.4c)-proxy(0.9) TopG TopG TopG" + '\n';
                    send(fd, one.c_str(), one.length(), 0);
                    send(fd, two.c_str(), two.length(), 0);
                    send(fd, tre.c_str(), tre.length(), 0);
                    send(fd, foor.c_str(), foor.length(), 0);

                }

            } 
            
           else if (startsWith(command, "JOIN ")) {
                std::string nick;
                for (size_t i = 0; i < _clients.size(); ++i) {
                    if (_clients[i].getFd() == fd) {
                        nick = _clients[i].getNick();
                        break;
                    }
                }
                std::string channelName, pass ;
                // std::string channelName = command.substr(6);
                std::istringstream iss(command.substr(5));
                iss >> channelName ;
                channelName = channelName.substr(1);
                channelName = trim(channelName);
                std::getline(iss, pass);
                pass = trim(pass);

                // Check if the channel already exists
                std::map<std::string, Channel>::iterator it = channels.find(channelName);
                if (it != channels.end()) {
                    // Channel already exists
                    if ((isinveted == 1 && channels[channelName].isInvited(nick)) || channels[channelName].isOperator(fd)) {
                        // User is invited, create the channel
                        std::cout << "ha huwa dkhaal l ******** lwla ***********" << std::endl;
                        createChannel(channelName, nick, fd);
                    } 
                    else if (isinveted == 0){
                        std::cout << "ha huwa dkhaal l ******** ltania ***********" << std::endl;
                        std::cout << "this is the geted pass " << channels[channelName].getPass() << std::endl;
                        std::cout << "IM PASS " << pass << std::endl; 
                        std::cout << "PASS MODE IS ON" << std::endl;
                        if (itHasPass == 1 && channels[channelName].getPass() == pass)
                            createChannel(channelName, nick, fd);
                        else if (itHasPass == 0)
                            createChannel(channelName, nick, fd);
                        else if (itHasPass == 1 && channels[channelName].getPass() != pass)
                            std::cout << "ZAPI " << std::endl;

                    }
        
                    else {
                        // User is not invited, send error message
                        std::string errorMessage = ":server.host NOTICE " + nick + " :Error: you are not invited\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.length(), 0);
                    }

                } 
                else 
                {

                    std::cout << "ha huwa dkhal l ********* tania *********** " << std::endl;
                    // Channel does not exist, create the channel
                    createChannel(channelName, nick, fd);
                }

            } 
            else if (startsWith(command, "PRIVMSG ")) 
            {
                    std::istringstream iss(command);
                    std::string cmd, recipient, message;
                    std::string niiick;
                    iss >> cmd >> recipient;
                    recipient = trim(recipient);
                    std::getline(iss, message);
                    message = trim(message);
                    message = message.substr(1);
                    if (recipient[0] == '#')
                    {

                        recipient = recipient.substr(1);
                        std::cout << "this the chanel name and was good : " << recipient << std::endl;
                        for (size_t i = 0; i < _clients.size(); ++i)
                        {
                            if (_clients[i].getFd() == fd)
                            {
                                niiick =  _clients[i].getNick();
                                break;
                            }
                        }
                        broadcastMessage(recipient, niiick, message, fd);
                    }
                    else
                    {
                        handlePrivateMessage(fd, recipient, message);
                    }
                    std::cout << "Recipient: " << recipient << std::endl;
                    std::cout << "Message: " << message << std::endl;
            }


            else if (startsWith(command, "KICK "))
            {
                std::string channelName, userToKick, reason;
                std::istringstream iss(command.substr(6));
                iss >> channelName >> userToKick;
                std::getline(iss, reason);
                channelName = trim(channelName);
                userToKick = trim(userToKick);
                reason = trim(reason);

                if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd))
                {
                    int userFd = channels[channelName].findUserFdForKickRegulars(userToKick);
                    if (userFd != -1) 
                    {
                        channels[channelName].ejectUserfromusers(userFd);
                        channels[channelName].ejectUserfromivited(userToKick);
                        std::string kickMessage = ":" + channels[channelName].getNickname(fd) + " KICK #" + channelName + " " + userToKick + " :" + reason + "\n";
                        smallbroadcastMessagefortheckick(channels[channelName].getNickname(fd), channelName, userToKick, reason);
                        send(fd, kickMessage.c_str(), kickMessage.length(), 0);
                        send(userFd , kickMessage.c_str(), kickMessage.length(), 0);
                    } 
                    else 
                    {
                        std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error: the user : " + userToKick + " is not found or offline." + "\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                    }
                }
                else 
                {
                    std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error1: You are not authorized to execute this command " + userToKick + "\r\n";
                    send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                }
            }


            else if (startsWith(command, "TOPIC "))
            {
                std::string channelName, topic;
                std::istringstream iss(command.substr(7));
                iss >> channelName;
                std::getline(iss, topic);
                channelName = trim(channelName);
                topic = trim(topic);
                topic = topic.substr(1);

                if ((channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) || issettop == 1)
                {
                    channels[channelName].setTopic(topic);
                    std::string topicMessage = ":" + channels[channelName].getNickname(fd) + " TOPIC #" + channelName + " :" + topic + "\n";
                    send(fd, topicMessage.c_str(), topicMessage.size(), 0);
                    smallbroadcastMessageforTopic(channels[channelName].getNickname(fd), channelName, topic );
                }
                else
                {
                    std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error2: You are not authorized to execute this command " + "\r\n";
                    send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                }


            }

            else if (startsWith(command, "INVITE "))
            {
                std::string channelName, nickname;
                std::istringstream iss(command.substr(7));
                iss >> nickname >> channelName;
                channelName = trim(channelName);
                nickname = trim(nickname);
                channelName = channelName.substr(1);
                if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd))
                {
                    channels[channelName].addClientinveted(nickname, fd);
                    std::cout << "this client is invited : " << nickname << std::endl;
                    handleInvitation(fd, nickname, channelName);
                }
                else
                {
                    std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error3: You are not authorized to execute this command " + "\r\n";
                    send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                }
            }


            else if (startsWith(command, "MODE "))
            {
                std::string channelName, mode , nick;
                std::istringstream iss(command.substr(6));
                iss >> channelName >> mode >> nick;
                channelName = trim(channelName);
                mode = trim(mode);
                nick = trim(nick);
                std::map<std::string, Channel>::iterator it = channels.find(channelName);


                std::cout << "this is the mode : " << mode << std::endl;
                if (mode == "+o")
                {
                    if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
                        channels[channelName].addOperator(nick, channels[channelName].getUserFd(nick));
                        std::string modeChangeMessage = ":server.host MODE #" + channelName + " " + mode + " by " + channels[channelName].getNickname(fd) + " and set " + nick + " as operator\n";
                        send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
                        smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
                    }
                    else
                    {
                        std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error: You are not authorized to execute this command " + "\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                    }

                }
                else if (mode == "-o")
                {
                    if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
                        channels[channelName].removeOperator(nick);
                        std::string modeChangeMessage = ":server.host MODE #" + channelName + " " + mode + " by " + channels[channelName].getNickname(fd) + " and set " + nick + " as operator\n";
                        send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
                        smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
                    }
                    else
                    {
                        std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error: You are not authorized to execute this command " + "\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                    }
                }
                else if (mode == "-t")
                {
                    if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
                        std::string modeChangeMessage = ":server.host MODE #" + channelName + " " + mode + " by " + channels[channelName].getNickname(fd) + "\n";
                        send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
                        smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
                        issettop = 1;
                    }
                    else
                    {
                        std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error: You are not authorized to execute this command " + "\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                    }
                }
                else if (mode == "+t")
                {
                    if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
                        std::string modeChangeMessage = ":server.host MODE #" + channelName + " " + mode + " by " + channels[channelName].getNickname(fd) + "\n";
                        send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
                        smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
                        issettop = 0;
                    }
                    else
                    {
                        std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error: You are not authorized to execute this command " + "\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                    }
                }
                else if (mode == "+i")
                {

                    if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)){
                        std::string modeChangeMessage = ":server.host MODE #" + channelName + " +i by " + channels[channelName].getNickname(fd) + "\n";
                        send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
                        smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
                        isinveted = 1;
                        
                    }
                    else if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd) == false)
                    {
                        std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error5: You are not authorized to execute this command " + "\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                    }
                    
                }
                else if (mode == "-i")
                {
                    if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)){
                        std::string modeChangeMessage = ":server.host MODE #" + channelName + " -i by " + channels[channelName].getNickname(fd) + "\n";
                        send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
                        smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
                        isinveted = 0;
                    }
                    else
                    {
                        std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error6: You are not authorized to execute this command " + "\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                    }
                }
                else if (mode == "-k")
                {
                    std::string ChanPass = (mode.substr(2));
                    if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)){
                        std::string modeChangeMessage = ":server.host MODE #" + channelName + " -k by " + channels[channelName].getNickname(fd) + "\n";
                        send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
                        smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
                        itHasPass = 0;
                    }
                    else
                    {
                        std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error7: You are not authorized to execute this command " + "\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                    }
                }
                else if (mode == "+k")
                {
                    // std::string ChanPass = (mo.substr(2));
                    nick = trim(nick);
                    std::cout << "HI IM TRIMMED PASS " << nick << std::endl;
                    channels[channelName].setPass(nick);
                    if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)){
                        std::string modeChangeMessage = ":server.host MODE #" + channelName + " +k by " + channels[channelName].getNickname(fd) + "\n";
                        send(fd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
                        smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
                        itHasPass = 1;
                    }
                    else
                    {
                        std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error8: You are not authorized to execute this command " + "\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                    }
                }
            }
//**************** STOOOOOOP HERE TOP G ... 
            break;
        }
    }

    if (bytesRead == 0) {
        std::cout << "Client <" << fd << "> Disconnected" << std::endl;
        clientCleanup(fd);
    } else if (bytesRead == -1) {
        std::cerr << "Error reading data from client <" << fd << ">" << std::endl;
        clientCleanup(fd);
    }
}


void Server::clientCleanup(int fd) {
    for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
        if (it->fd == fd) {
            _fds.erase(it);
            close(fd);
            break;
        }
    }

    for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->getFd() == fd) {
            _clients.erase(it);
            break;
        }
    }
}

void Server::closeFds() {
    for (size_t i = 0; i < _clients.size(); i++){
        int fd = _clients[i].getFd();
        std::cout << "Client <" << fd << "> Disconnected" << std::endl;
        close(fd);
    }

    if (_serverSocketFd != -1)
        close(_serverSocketFd);

    _fds.clear();
}