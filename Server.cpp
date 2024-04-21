#include "Server.hpp"
int a = 0;
int opperatorfd = 0;
int issettop = 0;
int isinveted = 0;


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
            std::string passwordRequest = "Please enter the password:\n";
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
    // Find the first non-whitespace character
    size_t first = str.find_first_not_of(" \t\n\r");

    // If the string is all whitespace, return an empty string
    if (std::string::npos == first) {
        return "";
    }

    // Find the last non-whitespace character
    size_t last = str.find_last_not_of(" \t\n\r");

    // Return the trimmed substring
    return str.substr(first, last - first + 1);
}

std::string formatCreationTime() {
    // Get the current time
    std::time_t currentTime = std::time(NULL);
    // Convert the current time to tm struct for easier manipulation
    std::tm* localTime = std::localtime(&currentTime);

    // Format the time string manually
    char buffer[80]; // Buffer to hold the formatted time string
    std::strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", localTime);
    return std::string(buffer);
}


std::string constructCreationTimeMessage(const std::string& channelName) {
    std::stringstream ss;
    ss << "Channel #" << channelName << " created " << formatCreationTime();
    return ss.str();
}
std::string constructJoinedTimeMessage(const std::string& channelName) {
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

        // Send JOIN message to the client
        std::string joinMessage = ":" + nickname + " JOIN #" + channelName + "\n";
        send(fd, joinMessage.c_str(), joinMessage.length(), 0);

        // Send MODE message to the client
        std::string modeMessage = ":irc.example.com MODE #" + channelName + " +nt\n";
        send(fd, modeMessage.c_str(), modeMessage.length(), 0);

        // Send NAMES message to the client
        std::string namesMessage = ":irc.example.com 353 " + nickname + " = #" + channelName + " :@" + nickname + "\n";
        send(fd, namesMessage.c_str(), namesMessage.length(), 0);

        // Send END OF NAMES message to the client
        std::string endOfNamesMessage = ":irc.example.com 366 " + nickname + " #" + channelName + " :End of /NAMES list.\n";
        send(fd, endOfNamesMessage.c_str(), endOfNamesMessage.length(), 0);

    std::string creationTimeMessage = constructCreationTimeMessage(channelName);
    std::string channelMessage = ":irc.example.com 354 " + channelName + " " + creationTimeMessage + "\n";
    send(fd, channelMessage.c_str(), channelMessage.length(), 0);

        // Insert the new channel into the map
        channels.insert(std::make_pair(channelName, newChannel));

    } else {
    // Channel already exists, just add the user to it
    it->second.addClient(nickname, fd);
    std::string operators = channels[channelName].getOperatorNickname(opperatorfd);

    // Send JOIN message to the client
    std::string joinMessage = ":" + nickname + " JOIN #" + channelName + "\n";
    send(fd, joinMessage.c_str(), joinMessage.length(), 0);

    // Send CHANNEL TOPIC message to the client
    std::cout << "this is the topi and he good :  "<< channels[channelName].getTopic()  << std::endl;
    std::string topicMessage = ":irc.example.com 332 " + nickname + " #" + channelName + " :" + channels[channelName].getTopic() + " https://irc.com\n";
    send(fd, topicMessage.c_str(), topicMessage.length(), 0);

    // Send CHANNEL CREATION TIME message to the client

    // Send NAMES message to the client
    std::string namesMessage = ":irc.example.com 353 " + nickname + " @ #" + channelName + " :";

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

    // Send END OF NAMES message to the client
    std::string endOfNamesMessage = ":irc.example.com 366 " + nickname + " #" + channelName + " :End of /NAMES list.\n";
    send(fd, endOfNamesMessage.c_str(), endOfNamesMessage.length(), 0);

    std::string creationTimeMessage = constructJoinedTimeMessage(channelName);
    std::string channelMessage = ":irc.example.com 354 " + channelName + " " + creationTimeMessage + "\n";
    send(fd, channelMessage.c_str(), channelMessage.length(), 0);

    smallbroadcastMessageforjoin(nickname, channelName);
}
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.substr(0, prefix.length()) == prefix;
}

 void Server::setNickname(int fd, const std::string& nickname) {
        nicknames[fd] = nickname; // Associate the nickname with the client's socket descriptor
    }

 void Server::setUsernameoperators(int fd, const std::string& username) {
        usernamesoperators[fd] = username; // Associate the nickname with the client's socket descriptor
    }
 void Server::setUsernames(int fd, const std::string& username) {
        usernames[fd] = username; // Associate the nickname with the client's socket descriptor
    }
 void Server::setUsernameregular(int fd, const std::string& username) {
        usernamesregulars[fd] = username; // Associate the nickname with the client's socket descriptor
    }

//it's a just a response for my client 
void sendResponse(int fd, const std::string& message) {
        // Convert the message string to a C-style string
        const char* msg = message.c_str();
        
        // Get the length of the message
        size_t len = strlen(msg);                 
        
        // Send the message to the client
        ssize_t bytesSent = send(fd, msg, len, 0);
        
        // Check if the send operation was successful
        if (bytesSent == -1) {
            // Handle send error
            // You can log an error message or take appropriate action
            std::cerr << "Error sending response to client <" << fd << ">" << std::endl;
        } else {
            // Output the sent message to the console
            std::cout << "Response sent to client <" << fd << ">: " << message << std::endl;
        }
    }



//here finding the users for the handling the privet msg 
int findUserFd(const std::string& nickname, const std::map<int, std::string>& usernames) {
    // Iterate over each entry in the usernames map
    for (std::map<int, std::string>::const_iterator it = usernames.begin(); it != usernames.end(); ++it) {
        // Check if the nickname matches
        if (it->second == nickname) {
            // Return the file descriptor associated with the nickname
            return it->first;
        }
    }
    // Return -1 if the nickname is not found
    return -1;
}


///iterate to find the username with the fd

// std::string Server::findUsernameforsending(int fd) {
//     std::map<int, std::string>::iterator it;
//     for (it = usernames.begin(); it != usernames.end(); ++it) {
//         if (it->first == fd) {
//             return it->second; // Return the file descriptor if the nickname matches
//         }
//     }
// }
//forget it

//handling privet msg between users only 
void Server::handlePrivateMessage(int senderFd, const std::string& recipient, const std::string& message) {
    // Find the recipient's connection (socket file descriptor)
    int recipientFd = findUserFd1(recipient);
    // std::string  sendernameuser = findUsernameforsending(senderFd);

    if (recipientFd != -1) {
        // Forward the private message to the recipient's client
        std::string privateMessage = ":" + nicknames[senderFd] + " PRIVMSG " + recipient + " :" + message + "\r\n";
        send(recipientFd, privateMessage.c_str(), privateMessage.length(), 0);
    } else {
        // Handle case where recipient is not found (e.g., user not online)
        std::string errorMessage = ":server.host NOTICE " + nicknames[senderFd] + " :Error: User '" + recipient + "' not found or offline\r\n";
        send(senderFd, errorMessage.c_str(), errorMessage.length(), 0);
    }
}

void Server::handleInvitation(int senderFd, const std::string& recipient, std::string channelName) {
    // Find the recipient's connection (socket file descriptor)
    int recipientFd = findUserFd1(recipient);
    // std::string  sendernameuser = findUsernameforsending(senderFd);

    if (recipientFd != -1) {
        // Construct the invitation message
        std::string inviteMessage = ":" + nicknames[senderFd] + " INVITE " + recipient + " :#" + channelName + "\r\n";

        // Send the invitation message to the recipient
        send(recipientFd, inviteMessage.c_str(), inviteMessage.length(), 0);
    } else {
        // Handle case where recipient is not found (e.g., user not online)
        std::string errorMessage = ":server.host NOTICE " + nicknames[senderFd] + " :Error: User '" + recipient + "' not found or offline\r\n";
        send(senderFd, errorMessage.c_str(), errorMessage.length(), 0);
    }
}

//this find is for finding nickname of the users i need to brodcasting to 
int Server::findUserFd1(const std::string& username) {
    std::map<int, std::string>::iterator it;
    for (it = nicknames.begin(); it != nicknames.end(); ++it) {
        if (it->second == username) {
            return it->first; // Return the file descriptor if the nickname matches
        }
    }
    return -1; // Return -1 if the nickname is not found
}

int Server::findUserFdforkickregulars(const std::string& username) {
    std::map<int, std::string>::iterator it;
    for (it = usernamesregulars.begin(); it != usernamesregulars.end(); ++it) {
        if (it->second == username) {
            return it->first; // Return the file descriptor if the nickname matches
        }
    }
    return -1; // Return -1 if the nickname is not found
}
// brodcasting msg to all nicks in the  channel 
void Server::broadcastMessage(const std::string& channel, const std::string& senderNickname, const std::string& msg, int fd) {
    // Check if the channel exists
    debugPrintChannels();
    std::map<std::string, Channel>::iterator it = channels.find(channel);
    if (it == channels.end()) {
        std::cerr << "Channel " << channel << " does not exist" << std::endl;
        return;
    }

    if (channels[channel].findUserFdForKickRegulars(senderNickname) == -1)
    {
        return;
    }
    // Construct the IRC message with the correct format for broadcasting
    std::string message = ":" + senderNickname + " PRIVMSG #" + channel + " :" + msg + "\r\n";


    // Get a reference to the vector of clients in the channel
    const std::vector<std::string>& clients = it->second.getClients();

    // Iterate over the vector of clients and send the message to each one
    for (size_t i = 0; i < clients.size(); ++i) {
        // Get the current client nickname
        const std::string& client = clients[i];

        // Skip sending the message to the sender
        std::cout << "this is the client name : "<< client <<  std::endl;
        std::cout << "this is the nickname name : " << senderNickname << std::endl;

        if (client == senderNickname) {
            continue;
        }

        // Find the file descriptor associated with the client nickname
        int recipientFd = it->second.getUserFd(client);

        // If the file descriptor is found, send the message to the client
        if (recipientFd != -1) {
            std::cout << message << std::endl;
            send(recipientFd, message.c_str(), message.size(), 0);
        } else {
            // If the file descriptor is not found, print an error message
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

    // Get a reference to the vector of clients in the channel
    const std::vector<std::string>& clients = it->second.getClients();

    // Iterate over the vector of clients and send the message to each one
    for (size_t i = 0; i < clients.size(); ++i) {
        // Get the current client nickname
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

    // Get a reference to the vector of clients in the channel
    const std::vector<std::string>& clients = it->second.getClients();

    // Iterate over the vector of clients and send the message to each one
    for (size_t i = 0; i < clients.size(); ++i) {
        // Get the current client nickname
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

    // Construct the topic message
    std::string topicMessage = ":" + nicknamesender + " TOPIC #" + channelname + " :" + topic + "\n";

    // Get a reference to the vector of clients in the channel
  const std::vector<std::string>& clients = it->second.getClients();

    // Iterate over the vector of clients and send the message to each one
    for (size_t i = 0; i < clients.size(); ++i) {
        // Get the current client nickname
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

    // Construct the mode change message
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

    // Get the file descriptor of the sender
    int senderFd = it->second.getUserFd(nicknamesender);

    // Get a reference to the vector of clients in the channel
    const std::vector<std::string>& clients = it->second.getClients();

    // Iterate over the vector of clients and send the message to each one
    for (size_t i = 0; i < clients.size(); ++i) {
        // Get the current client nickname
        const std::string& client = clients[i];

        int recipientFd = it->second.getUserFd(client);
        if (recipientFd != -1 && recipientFd != senderFd) {
            send(recipientFd, modeChangeMessage.c_str(), modeChangeMessage.size(), 0);
        } else {
            std::cerr << "Client " << client << " not found or is the sender" << std::endl;
        }
    }
}




//check if ope or not
// bool Server::isOperator(int fd) {
//     // Check if the file descriptor exists in the map of operators
//     int fd = 
// }


void Server::kickUser(int fd) {
    // Close the socket connection for the given file descriptor
    close(fd);

    // Remove the user from any data structures tracking active connections
    // For example, if you have a map of file descriptors to usernames:
    std::map<int, std::string>::iterator it = usernamesregulars.find(fd);
    if (it != usernamesregulars.end()) {
        // Remove the user from the map
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
                    std::string errorMessage = "Error: Incorrect password\n";
                    send(fd, errorMessage.c_str(), errorMessage.length(), 0);
                }
                else {
                    std::string confirmation = "Welcome sir\n";
                    send(fd, confirmation.c_str(), confirmation.length(), 0);
                    a = 1;
                }

                // for (size_t i = 0; i < _clients.size(); ++i) {
                //     if (_clients[i].getFd() == fd) {
                //         _clients[i].setPassword(password);
                //         std::cout << "Password set for client " << fd << ": " << password << std::endl;
                //         break;
                //     }
                // }
            }
            else if (startsWith(command, "PING"))
            {
                std::istringstream iss(command);

                std::string serverHostname = command.substr(5); // Skip the "PING " prefix

// Construct the PONG message with the server hostname
                std::string pongMessage = "PONG " + serverHostname + "\r\n";

// Send the PONG message back to the client
                send(fd, pongMessage.c_str(), pongMessage.length(), 0);
                std::cout << "ping was sent" << std::endl;
            }
            else if (startsWith(command, "nick") && a == 1) {
                std::string cmd, nick;
                std::istringstream iss(command);
                iss >> cmd >> nick;
                nick = trim(nick);
                setNickname(fd, nick);
                for (size_t i = 0; i < _clients.size(); ++i) {
                    if (_clients[i].getFd() == fd) {
                        _clients[i].setNick(nick);
                        std::cout << "Password set for client " << fd << ": " << nick << std::endl;
                        break;
                    }
                }
                // Extract the nickname from the command
                // std::string nickname = command.substr(8); // Assuming "/setnick " is 9 characters long

                // Handle setting the nickname for the client's connection


                // Send a response back to the client confirming the action
                sendResponse(fd, "Nickname set to: " + nick + '\n');
                a = 2;
            } else if (startsWith(command, "user") && a == 2) {

                std::istringstream iss(command);
                std::string cmd, username, dontworry, dontworry1, realname, nickname;
                iss >> cmd >> username >> dontworry >> dontworry1 >> realname;
                // Remove leading and trailing whitespace from parameters
                username = trim(username);
                dontworry = trim(dontworry);
                dontworry1 = trim(dontworry1);
                realname = trim(realname);

                setUsernames(fd, username);

                for (size_t i = 0; i < _clients.size(); ++i) {
                    if (_clients[i].getFd() == fd) {
                        _clients[i].setUser(username);
                        _clients[i].setName(realname);
                        nickname = _clients[i].getNick();
                        break;
                    }
                }
                a = 0;

                sendResponse(fd, "username set to: " + username + '\n');
                sendResponse(fd, "realname set to: " + realname + '\n');

                std::string one = ":irc.l9oroch 001 " + nickname + " :Welcome to the l9oroch Network, " + nickname + '\n';
                std::string two = ":irc.l9oroch 002 " + nickname + " :Your host is l9oroch, running version 3030" + '\n';
                std::string tre = ":irc.l9oroch 003 " + nickname + " :This server was created Tue Nov 30 2011 at 11:11:25 EET" + '\n';
                std::string foor = ":irc.l9oroch 004 " + nickname + " l9oroch tella(enterprise)-2.3(12)-netty(5.4c)-proxy(0.9) oOiwscrknfbghexzSjFI bhijklmMnoOstvcdSuU bkohv" + '\n';
                send(fd, one.c_str(), one.length(), 0);
                send(fd, two.c_str(), two.length(), 0);
                send(fd, tre.c_str(), tre.length(), 0);
                send(fd, foor.c_str(), foor.length(), 0);



                




                // if (privilege_level == "operator" )
                // {
                //     //si moskir hna atbda lkhdma dyalk 
                //     // std::cout << "we need to handle this " << std::endl;
                //     setUsernameoperators(fd, username);
                //     sendResponse(fd, "Username set to: " + username + " with privilege_level : " + privilege_level + '\n');
                // }
                // else if (privilege_level == "regular") {
                //     setUsernameregular(fd, username);
                //     sendResponse(fd, "Username set to: " + username + " with privilege_level : " + privilege_level + '\n');

                // }
                

                // Process other commands or messages
                // processCommand(fd, command);
            } else if (startsWith(command, "JOIN ")) {
                std::string nick;
                for (size_t i = 0; i < _clients.size(); ++i) {
                    if (_clients[i].getFd() == fd) {
                        nick = _clients[i].getNick();
                        break;
                    }
                }

                std::string channelName = command.substr(6);
                channelName = trim(channelName);

                // Check if the channel already exists
                std::map<std::string, Channel>::iterator it = channels.find(channelName);
                if (it != channels.end()) {
                    // Channel already exists
                    if ((isinveted == 1 && it->second.isInvited(nick)) || it->second.isOperator(fd)) {
                        // User is invited, create the channel
                        std::cout << "ha huwa dkhaal l ******** lwla ***********" << std::endl;
                        createChannel(channelName, nick, fd);
                    } 
                    else if (isinveted == 0){
                        std::cout << "ha huwa dkhaal l ******** ltania ***********" << std::endl;
                        createChannel(channelName, nick, fd);

                    }
                    else {
                        // User is not invited, send error message
                        std::string errorMessage = ":server.host NOTICE " + nick + " :Error: you are not invited\r\n";
                        send(fd, errorMessage.c_str(), errorMessage.length(), 0);
                    }
                } else {

                    std::cout << "ha huwa dkhal l ********* tania *********** " << std::endl;
                    // Channel does not exist, create the channel
                    createChannel(channelName, nick, fd);
                }

            } else if (startsWith(command, "PRIVMSG ")) {
                    // Extract the recipient and the message from the command
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
                        for (size_t i = 0; i < _clients.size(); ++i) {
                            if (_clients[i].getFd() == fd) {
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
            else if (startsWith(command, "KICK ")) {
    // Extract the channel name and user to be kicked
                std::string channelName, userToKick, reason;
                std::istringstream iss(command.substr(6));
                iss >> channelName >> userToKick;
                std::getline(iss, reason);
                channelName = trim(channelName);
                userToKick = trim(userToKick);
                reason = trim(reason);

    // Check if the sender is an operator in the specified channel
                if (channels.find(channelName) != channels.end() && channels[channelName].isOperator(fd)) {
        // Check if the user to be kicked is online
                    int userFd = channels[channelName].findUserFdForKickRegulars(userToKick);
                    if (userFd != -1) {
            // Kick the user
                        // kickUser(userFd);
                        channels[channelName].ejectUser(userFd);
                        std::string kickMessage = ":" + channels[channelName].getNickname(fd) + " KICK #" + channelName + " " + userToKick + " :" + reason + "\n";
                        smallbroadcastMessagefortheckick(channels[channelName].getNickname(fd), channelName, userToKick, reason);
                        send(fd, kickMessage.c_str(), kickMessage.length(), 0);
                        send(userFd , kickMessage.c_str(), kickMessage.length(), 0);
                    } else {
                             std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error: the user : " + userToKick + " is not found or offline." + "\r\n";

                        send(fd, errorMessage.c_str(), errorMessage.size(), 0);
                    }
             } else {
                std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error1: You are not authorized to execute this command " + userToKick + "\r\n";
                send(fd, errorMessage.c_str(), errorMessage.size(), 0);             }
            }
            else if (startsWith(command, "TOPIC ")){
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
                    smallbroadcastMessageforTopic(channels[channelName].getNickname(fd), channelName, topic );
                } else {
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
                } else {
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
                        smallbroadcastMOOD(channels[channelName].getNickname(fd), channelName, mode, nick);
                        isinveted = 0;
                    }
                    else
                    {
                        std::string errorMessage = ":" + channels[channelName].getNickname(fd) + " PRIVMSG #" + channelName + " :Error6: You are not authorized to execute this command " + "\r\n";
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