#ifndef CLIENT_HPP
#define CLIENT_HPP

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

class Client {
    private:
        std::string userName;
        std::string nickName;
        std::string hostName;
        std::string realName;
        int clientSocket;
        int port;
    public:
        Client();
        Client(std::string ip, int port);
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

        std::string getUserName(void) const;
        std::string getNickName(void) const;
        std::string getHostName(void) const;
        std::string getRealName(void) const;
        int getClientSocket(void) const;

        void setUpSocket(void);
        void connect(void);
        void messaging(void);
        void disconnect(void);

};

#endif