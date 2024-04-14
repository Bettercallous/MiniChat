#include "Server.hpp"

bool Server::_signal = false;

Server::Server() {}

Server::~Server() {}

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

            addPollfd(newFd, POLLIN, 0);
            _clients.push_back(Client(newFd, inet_ntoa((client_addr.sin_addr))));

            std::cout << "Client <" << newFd << "> Connected" << std::endl;
        }
    }
}


// FUNCTIONS OF TOP GGGGGG.. START FROM HEEEREEE 




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

void Server::createChannel(const std::string& channelName, const std::string& nickname) {
    // Check if the channel already exists
    if (channels.find(channelName) == channels.end()) {
        // Channel doesn't exist, so create it
        std::vector<std::string> users;
        users.push_back(nickname);  // Add the user to the channel
        channels[channelName] = users;
        std::cout << "Channel '" << channelName << "' created by '" << nickname << "'" << std::endl;
    } else {
        // Channel already exists, just add the user to it
        channels[channelName].push_back(nickname);
        std::cout << "User '" << nickname << "' joined channel '" << channelName << "'" << std::endl;
    }
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.substr(0, prefix.length()) == prefix;
}

 void Server::setNickname(int fd, const std::string& nickname) {
        nicknames[fd] = nickname; // Associate the nickname with the client's socket descriptor
    }

 void Server::setUsername(int fd, const std::string& username) {
        usernames[fd] = username; // Associate the nickname with the client's socket descriptor
    }

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



void Server::handlePrivateMessage(int senderFd, const std::string& recipient, const std::string& message) {
    // Find the recipient's connection (socket file descriptor)
    int recipientFd = findUserFd(recipient, usernames);

    if (recipientFd != -1) {
        // Forward the private message to the recipient's client
        std::string privateMessage = "PRIVATE " + usernames[senderFd] + ": " + message + "\n";
        send(recipientFd, privateMessage.c_str(), privateMessage.length(), 0);
    } else {
        // Handle case where recipient is not found (e.g., user not online)
        std::string errorMessage = "Error: User '" + recipient + "' not found or offline\n";
        send(senderFd, errorMessage.c_str(), errorMessage.length(), 0);
    }
}

int Server::findUserFd1(const std::string& nickname) {
    std::map<int, std::string>::iterator it;
    for (it = nicknames.begin(); it != nicknames.end(); ++it) {
        if (it->second == nickname) {
            return it->first; // Return the file descriptor if the nickname matches
        }
    }
    return -1; // Return -1 if the nickname is not found
}

void Server::broadcastMessage(const std::string& channel, const std::string& senderNickname, const std::string& msg) {
    // Check if the channel exists
    if (channels.find(channel) == channels.end()) {
        std::cerr << "Channel " << channel << " does not exist" << std::endl;
        return;
    }

    // Iterate through all clients in the channel and send the message
// Get a reference to the vector of clients in the channel
    const std::vector<std::string>& clients = channels[channel];

    // Iterate over the vector using iterators
    std::vector<std::string>::const_iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        // Get the current client nickname
        const std::string& client = *it;

        // Find the file descriptor associated with the client nickname
        int recipientFd = findUserFd1(client);

        // If the file descriptor is found, send the message to the client
        if (recipientFd != -1) {
            std::string message = "PRIVMSG " + channel + " :" + senderNickname + ": " + msg + "\r\n";
            send(recipientFd, message.c_str(), message.size(), 0);
        } else {
            // If the file descriptor is not found, print an error message
            std::cerr << "Client " << client << " not found" << std::endl;
        }
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
            if (startsWith(command, "/setnick ")) {
                // Extract the nickname from the command
                std::string nickname = command.substr(9); // Assuming "/setnick " is 9 characters long

                // Handle setting the nickname for the client's connection
                setNickname(fd, nickname);

                // Send a response back to the client confirming the action
                sendResponse(fd, "Nickname set to: " + nickname);
            } else if (startsWith(command, "/setuser ")) {
                std::string username = command.substr(9);

                setUsername(fd, username);

                sendResponse(fd, "Username set to: " + username);

                // Process other commands or messages
                // processCommand(fd, command);
            } else if (startsWith(command, "/join ")) {
                std::string chanelname = command.substr(5);
                chanelname = trim(chanelname);
                createChannel(chanelname, nicknames[fd]);

            } else if (startsWith(command, "PRIVMSG ")) {
                    // Extract the recipient and the message from the command
                    std::istringstream iss(command);
                    std::string cmd, recipient, message;
                    iss >> cmd >> recipient;
                    recipient = trim(recipient);
                    std::getline(iss, message);
                    message = trim(message);
                    if (recipient[0] == '#')
                    {
                        recipient = recipient.substr(1);
                        std::cout << "this the chanel name and was good : " << recipient << std::endl;
                        broadcastMessage(recipient, nicknames[fd], message);
                    }
                    else
                    {
                     if (message[0] == ':') {
                            message = message.substr(1);
                            handlePrivateMessage(fd, recipient, message);
                    }
                        
                    }
                    // Remove the colon from the recipient if present
                    // if (!recipient.empty() && recipient[1] == ':') {
                    //     recipient = recipient.substr(1);
                    // }
                    

                    // Read the rest of the line as the message

                    // if (message[0] == ':')
                    //     message = message.substr(1);

                    // Print the extracted recipient and message for debugging
                    std::cout << "Recipient: " << recipient << std::endl;
                    std::cout << "Message: " << message << std::endl;
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