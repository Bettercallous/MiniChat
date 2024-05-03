#include "Server.hpp"

void Server::parseArgs(int ac, char **av) {
    if (ac != 3)
        throw std::runtime_error("Usage: ./ircserv <port> <password>");
    std::string port(av[1]);
    std::string pwd(av[2]);
    if (port.empty() || port.find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("Error: Invalid arguments");

    long _port = atol(av[1]);
    if (!(_port >= 0 && _port <= 65535))
        throw std::runtime_error("Error: Invalid arguments");

    if (pwd.empty())
        throw std::runtime_error("Error: Invalid arguments");

    this->_port = _port;
    this->_password = pwd;
}

std::string Server::intToString(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

std::string Server::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

bool Server::startsWith(const std::string& str, const std::string& prefix) {
    return str.substr(0, prefix.length()) == prefix;
}

int Server::stringToInt(const std::string& str) {
    std::stringstream ss(str);
    int result;
    ss >> result;
    return result;
}

bool Server::isValidPassword(const std::string& passwordLine) {
    if (!passwordLine.empty() && 
        ((passwordLine[0] == '"' && passwordLine.size() > 1 && passwordLine[passwordLine.size() - 1] == '"') ||
         (passwordLine[0] == '\'' && passwordLine.size() > 1 && passwordLine[passwordLine.size() - 1] == '\''))) {
        return true;
    }
    return false;
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

void Server::closeFds()
{
    for (size_t i = 0; i < _clients.size(); i++){
        int fd = _clients[i].getFd();
        std::cout << "Client <" << fd << "> Disconnected" << std::endl;
        close(fd);
    }

    if (_serverSocketFd != -1)
        close(_serverSocketFd);
    _fds.clear();
}

void Server::cleanChannel(int fd) {
    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        Channel& channel = it->second;
        std::string nickname = channel.getNickname(fd);
        if (channel.isUserInChannel(nickname)) {
            channel.ejectUserfromusers(fd);
        }
    }
    
    std::map<int, std::string>::iterator userIt;
    userIt = usernames.find(fd);
    if (userIt != usernames.end()) {
        usernames.erase(userIt);
    }
    
    std::map<int, std::string>::iterator nickIt;
    nickIt = nicknames.find(fd);
    if (nickIt != nicknames.end()) {
        nicknames.erase(nickIt);
    }
    
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        std::map<std::string, int> &usersfdmap = it->second.getUserFdMap();
        for (std::map<std::string, int>::iterator it2 = usersfdmap.begin(); it2 != usersfdmap.end(); ++it2) {
            if (it2->second == fd)
                usersfdmap.erase(it2);
        }
    }
    for (std::map<std::string, Channel>::iterator it1 = channels.begin(); it1 != channels.end(); ++it1) {
        std::map<std::string, int> &invitedusrmap = it1->second.invitedUserss();
        for (std::map<std::string, int>::iterator it2 = invitedusrmap.begin(); it2 != invitedusrmap.end(); ++it2) {
            if (it2->second == fd)
                invitedusrmap.erase(it2);
        }
    }
    for (std::map<std::string, Channel>::iterator it2 = channels.begin(); it2 != channels.end(); ++it2) {
        std::map<std::string, int> &operatorsmap = it2->second.getOperators();
        for (std::map<std::string, int>::iterator it2 = operatorsmap.begin(); it2 != operatorsmap.end(); ++it2) {
            if (it2->second == fd)
                operatorsmap.erase(it2);
        }
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

int Server::randomInRange(int min, int max)
{
    if (min > max) {
        return -1;
    }

    int range_size = max - min + 1;
    double random_double = (double)rand() / (RAND_MAX + 1.0) * range_size;

    return (int)random_double + min;
}

// find nicknames of the users to brodcast to 
int Server::findUserFd1(const std::string& username) {
    std::map<int, std::string>::iterator it;
    for (it = nicknames.begin(); it != nicknames.end(); ++it) {
        if (it->second == username)
            return it->first; 
    }
    return -1;
}

bool Server::dontputthesamenick(const std::string& nickname) {
    std::map<int, std::string>::iterator it;
    for (it = nicknames.begin(); it != nicknames.end(); ++it) {
        if (it->second == nickname)
            return true;
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