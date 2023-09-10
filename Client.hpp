#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Errors.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <thread>
#include <pthread.h>
#include <algorithm>
#include <cstdio>
#include <sys/poll.h>
#include <fcntl.h>

class Client {
    private:
        std::string userName;
        std::string nickName;
        std::string hostName;
        std::string realName;
        std::string password;
        std::string serverHostName;
    public:
        std::string _buffer;
        std::time_t timeJoined;
        int clientSocket;
        bool logged;
        bool isRegistered;
        Client();
        Client(int fd);
        Client(const Client& copy);
        ~Client();

        Client& operator=(const Client& right);

        class Error: public std::exception {
        public:
            virtual const char* what() const throw();
        };

        void setUserName(std::string username);
        void setNickName(std::string nickname);
        void setHostName(std::string hostname);
        void setRealName(std::string realname);
        void setClientSocket(int socket);
        void setPassWord(std::string passwd);
        void setServerHostName(std::string hostname);

        std::string getUserName(void) const;
        std::string getNickName(void) const;
        std::string getHostName(void) const;
        std::string getRealName(void) const;
        std::string getServerHostName(void) const;
        int getClientSocket(void) const;
        std::string getPassWord(void) const;
        std::string getPrefixClient(void) const;
        const std::string getJoiningTime(void) const;

        void ClientToClientPrefix(std::string message);
        void ServerToClientPrefix(std::string message);
        void mySend(std::string message);
        void welcome();

};

#endif