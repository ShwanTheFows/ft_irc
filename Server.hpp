#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
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
#include <thread>
#include <pthread.h>
#include <algorithm>
#include <cstdio>
#include <sys/poll.h>

class Server {
    private:
        int serverSocket;
        std::string ip;
        int port;
        std::string password;
        std::vector<int> clientSockets;

        void setUpSocket(void);
        void bind(void);
        void listen(void);
        void handleClient(int clientSocket);
        void disconnect(void);
    public:
        Server();
        Server(std::string ip,int port, std::string password);
        Server(const Server& copy);
        ~Server();

        Server& operator=(const Server& right);

        class Error: public std::exception {
        public:
            virtual const char* what() const throw();
        };

        void setServerSocket(int socket);
        void setPort(int p);
        void setIp(int ip_addr);

        int getServerSocket(void) const;
        int getPort(void) const;
        std::string getIp(void) const;

        void run(void);
};

int parsing(const std::string& str);

#endif