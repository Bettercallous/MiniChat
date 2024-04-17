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
        std::string _addr;
        bool _isRegistered;
    public:
        Client();
        Client(int fd, std::string addr);
        ~Client();

        int getFd() const;
        std::string getPassowrd() const;
        void setPassword(const std::string& password); // Function to set the password
     // Getter and setter for name
        std::string getName() const;
        void setName(const std::string& newName);

        // Getter and setter for nick
        std::string getNick() const;
        void setNick(const std::string& newNick);

        // Getter and setter for user
        std::string getUser() const;
        void setUser(const std::string& newUser);

};

#endif