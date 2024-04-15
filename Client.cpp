#include "Client.hpp"

Client::Client() : _isRegistered(false) {}

Client::Client(int fd, std::string addr) : _fd(fd), _addr(addr) {}

Client::~Client() {}

int Client::getFd() const {return _fd;}
