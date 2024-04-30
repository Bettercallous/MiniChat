#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client {
    private:
        int _fd;
        std::string pass;
        std::string name;
        std::string nick;
        std::string user;
        std::string command;
        int _authentication;
    public:
        Client();
        Client(int fd);
        ~Client();

        int getFd() const;
        std::string getPassowrd() const;
        void setPassword(const std::string& password);

        std::string getName() const;
        void setName(const std::string& newName);

        std::string getNick() const;
        void setNick(const std::string& newNick);

        std::string getUser() const;
        void setUser(const std::string& newUser);
        
        int getAuthentication() const;
        void setAuthentication(int auth);

        const std::string& getCommand() const;

        void clearCommand();

        void appendCommand(std::string str);
        void setCommand(std::string cmd);
};

#endif