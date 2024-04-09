#include "Client.hpp"

Client::Client() {}

Client::Client(int fd, std::string addr) : _fd(fd), _addr(addr) {}

Client::~Client() {}

int Client::getFd() const {return _fd;}
