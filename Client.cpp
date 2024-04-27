#include "Client.hpp"

Client::Client() : _isRegistered(false) {}

Client::Client(int fd) : _fd(fd) {}

Client::~Client() {}

int Client::getFd() const {return _fd;}

void Client::setPassword(const std::string& password) {
    pass = password;
}

std::string Client::getPassowrd() const {
    return pass;
}

// Implementation of getter and setter functions for name
std::string Client::getName() const {
    return name;
}

void Client::setName(const std::string& newName) {
    name = newName;
}

// Implementation of getter and setter functions for nick
std::string Client::getNick() const {
    return nick;
}

void Client::setNick(const std::string& newNick) {
    nick = newNick;
}

// Implementation of getter and setter functions for user
std::string Client::getUser() const {
    return user;
}

void Client::setUser(const std::string& newUser) {
    user = newUser;
}