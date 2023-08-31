#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Errors.hpp"

class Server {
    private:
        int serverSocket;
        std::string ip;
        int port;
        std::string password;
        std::map<int, Client> clients;

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
        bool checkCommand(Client& client, std::string buffer);
        void checkPassword(Client& client, std::vector<std::string>& arguments);
        void checkNickName(Client& client, std::vector<std::string>& arguments);
};

int parsing(const std::string& str);

#endif