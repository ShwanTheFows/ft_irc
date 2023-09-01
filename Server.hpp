#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"

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
        std::string timeOfCreation;
        typedef void (Server::*cmd)(Client& client, std::vector<std::string>& arguments);
        std::map<std::string, cmd> cmdMap;

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

        void cmdMapinit(void);

        bool checkLoginCommands(Client& client, std::string buffer);
        void checkPassword(Client& client, std::vector<std::string>& arguments);
        void checkNickName(Client& client, std::vector<std::string>& arguments);
        void checkUserCommand(Client& client, std::vector<std::string>& arguments);
        bool checkCommands(Client& client, std::string buffer);

        void join(Client& client, std::vector<std::string>& arguments);
        void kick(Client& client, std::vector<std::string>& arguments);
        void part(Client& client, std::vector<std::string>& arguments);
        void notice(Client& client, std::vector<std::string>& arguments);
        void privmsg(Client& client, std::vector<std::string>& arguments);
        void quit(Client& client, std::vector<std::string>& arguments);
        void topic(Client& client, std::vector<std::string>& arguments);
        void names(Client& client, std::vector<std::string>& arguments);
        void list(Client& client, std::vector<std::string>& arguments);
        void invite(Client& client, std::vector<std::string>& arguments);
        void mode(Client& client, std::vector<std::string>& arguments);
        void oper(Client& client, std::vector<std::string>& arguments);
        void wallops(Client& client, std::vector<std::string>& arguments);
        void whois(Client& client, std::vector<std::string>& arguments);
};

#endif