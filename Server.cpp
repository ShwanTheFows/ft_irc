#include "Server.hpp"

Server::Server() {}

Server::Server(std::string ip, int port, std::string password) : ip(ip), port(port), password(password) {}

Server::Server(const Server& copy) {*this = copy;}

Server::~Server() {}

Server& Server::operator=(const Server& right) {
    if (this != &right) {
        this->serverSocket = right.getServerSocket();
    }

    return *this;
}

const char* Server::Error::what() const throw() {
    return "Error!";
}

void Server::setServerSocket(int socket) {this->serverSocket = socket;}
void Server::setPort(int p) {this->port = p;}
void Server::setIp(int ip_addr) {this->ip = ip_addr;}

int Server::getServerSocket(void) const {return this->serverSocket;}
int Server::getPort(void) const {return this->port;}
std::string Server::getIp(void) const {return this->ip;}

void Server::cmdMapinit(void) {
    std::string cmd_strings[] = {"join", "kick", "part", "notice", "privmsg", "quit"
    , "topic", "names", "list", "invite", "mode", "oper", "wallops", "whois"};
    cmd cmd_ptrs[] = {&Server::join, &Server::kick, 
    &Server::part, &Server::notice, &Server::privmsg, 
    &Server::quit, &Server::topic, &Server::names, 
    &Server::list, &Server::invite, &Server::mode, 
    &Server::oper, &Server::wallops, &Server::whois};
    int num_cmds = sizeof(cmd_ptrs) / sizeof(cmd);

    for (int i = 0; i < num_cmds; i++)
        cmdMap.insert(std::make_pair(cmd_strings[i], cmd_ptrs[i]));
}

void Server::run() {
    time_t now = time(0);
    tm *gmtm = gmtime(&now);
    timeOfCreation = asctime(gmtm);
    timeOfCreation.erase(std::remove(timeOfCreation.begin(), timeOfCreation.end(), '\n'), timeOfCreation.end());
    setUpSocket();
    bind();
    listen();
    cmdMapinit();

    std::vector<pollfd> fds(1);
    fds[0].fd = this->serverSocket;
    fds[0].events = POLLIN;

    while (true) {
        int result = poll(fds.data(), fds.size(), -1);

        if (result == -1) {
            std::cerr << "poll() error" << std::endl;
            break;
        }

        if (fds[0].revents & POLLIN) {
            int clientSocket = accept(this->serverSocket, NULL, NULL);

            if (clientSocket == -1) {
                std::cout << "Accept failed!" << std::endl;
                continue;
            }

            std::cout << "Accepted connection." << std::endl;
            this->clients[clientSocket] = Client(clientSocket);
            this->clients[clientSocket].setServerHostName(this->ip);

            fds.resize(this->clients.size() + 1);
            fds[this->clients.size()].fd = clientSocket;
            fds[this->clients.size()].events = POLLIN;
        }

        for (size_t i = 1; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                handleClient(fds[i].fd);
            }
        }
    }

    disconnect();
}

void Server::setUpSocket() {
    this->serverSocket = -1;
    this->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->serverSocket == -1) {
        std::cout << "Error in setting up the server socket!" << std::endl;
        throw std::runtime_error("Server socket setup failed.");
    } else {
        std::cout << "The server socket has been set up successfully!" << std::endl;
    }
}

void Server::bind() {
    sockaddr_in service;
    service.sin_family = AF_INET;
    if (inet_pton(AF_INET, this->ip.c_str(), &(service.sin_addr)) <= 0) {
        throw std::runtime_error("Invalid server address.");
    }
    service.sin_port = htons(this->port);
    if (::bind(this->serverSocket, reinterpret_cast<struct sockaddr*>(&service), sizeof(service)) != 0) {
        throw std::runtime_error("Binding failed.");
    } else {
        std::cout << "Binding is done successfully!" << std::endl;
    }
}

void Server::listen() {
    if (::listen(this->serverSocket, 1) == -1) {
        throw std::runtime_error("Listening failed.");
    } else {
        std::cout << "Listening for clients..." << std::endl;
    }
}

void Server::handleClient(int clientSocket) {
    int bufsize = 512;
    char buffer[bufsize];

    memset(buffer, 0, bufsize);  // Clear the buffer

    // Receive message from the client
    int bytesRead = recv(clientSocket, buffer, bufsize - 2, 0);

    if (bytesRead > 0) {
        // Print the received message
        removeTrailingNewline(buffer);
        if (!this->checkLoginCommands(clients[clientSocket], buffer)) {
            if (this->clients[clientSocket].isRegistered && !this->checkCommands(clients[clientSocket], buffer))
                clients[clientSocket].ServerToClientPrefix(ERR_UNKNOWNCOMMAND(clients[clientSocket].getNickName(), buffer));
        }
    } else if (bytesRead == 0) {
        std::cout << "Client " << clientSocket << " disconnected." << std::endl;
        close(clientSocket);
        // Remove the client socket from the map
        std::map<int, Client>::iterator it = this->clients.find(clientSocket);
        if (it != this->clients.end())
            this->clients.erase(it);
    }
}

bool Server::checkCommands(Client& client, std::string buffer) {
    std::vector<std::string> arguments = splitString(buffer);
    std::map<std::string, cmd>::iterator it;

    it = this->cmdMap.find(tolower(arguments[0]));

    if (it == cmdMap.end())
        return false;
    else {
        (this->*(it->second))(client, arguments);
        return true;
    }
}

bool Server::checkLoginCommands(Client& client, std::string buffer) {
    std::vector<std::string> arguments = splitString(buffer);

    if (arguments.empty()) return true;
    else if (tolower(trim(arguments[0])) == "pass") {
        checkPassword(client, arguments);
        return true;
    } else if (tolower(trim(arguments[0])) == "nick") {
        if (!client.logged)
            client.ServerToClientPrefix(ERR_NOTREGISTERED(client.getNickName()));
        else
            checkNickName(client, arguments);
        return true;
    } else if (tolower(trim(arguments[0])) == "user") {
        if (!client.logged)
            client.ServerToClientPrefix(ERR_NOTREGISTERED(client.getNickName()));
        else
            checkUserCommand(client, arguments);
        return true;
    }
    else return false;
}

void Server::checkPassword(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() > 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (client.isRegistered) client.ServerToClientPrefix(ERR_ALREADYREGISTRED(client.getNickName()));
    else if (client.getPassWord().empty() && this->password.length() != 0) {
        if (trim(arguments[1]) != this->password) client.ServerToClientPrefix(ERR_PASSWDMISMATCH(client.getNickName()));
        else if (trim(arguments[1]) == this->password) {
            client.setPassWord(trim(arguments[1]));
            client.logged = true;
        }
    }
}

void Server::checkNickName(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() > 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (client.isRegistered) client.ServerToClientPrefix(ERR_ALREADYREGISTRED(client.getNickName()));
    else if (arguments[1].empty()) client.ServerToClientPrefix(ERR_NONICKNAMEGIVEN(client.getNickName()));
    else if (client.getNickName().empty()) {
        if (!isValidNickname(trim(arguments[1]))) client.ServerToClientPrefix(ERR_ERRONEUSNICKNAME(client.getNickName()));
        else {
            for (std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); it++) {
                if (it->second.getNickName() == trim(arguments[1])) {
                    client.ServerToClientPrefix(ERR_NICKCOLLISION(client.getNickName()));
                    return ;
                }
            }
            client.setNickName(trim(arguments[1]));
            if (!client.getPassWord().empty() && !client.getNickName().empty() && !client.getUserName().empty()) {
                client.isRegistered = true;
                client.welcome(timeOfCreation);
            }
        }
    }
    else if (client.getNickName().length() != 0) client.ServerToClientPrefix(ERR_NICKNAMEINUSE(client.getNickName()));
}

void Server::checkUserCommand(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 5) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (client.isRegistered) client.ServerToClientPrefix(ERR_ALREADYREGISTRED(client.getNickName()));
    else if (arguments[1].empty()) client.ServerToClientPrefix(ERR_NONICKNAMEGIVEN(client.getNickName()));
    else if (client.getUserName().empty()) {
        if (!isValidUsername(trim(arguments[1]))) client.ServerToClientPrefix(ERR_ERRONEUSNICKNAME(client.getNickName()));
        else client.setUserName(trim(arguments[1]));
        client.setRealName(joinVectorFromIndex(arguments, 4));
        if (!client.getPassWord().empty() && !client.getNickName().empty() && !client.getUserName().empty()) {
            client.isRegistered = true;
            client.welcome(timeOfCreation);
        }
    }
}

void Server::disconnect() {
    close(this->serverSocket);
    for (std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
        close(it->second.getClientSocket());
    this->clients.clear();
}

void Server::join(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (!isValidChannelName(trim(arguments[1]))) client.ServerToClientPrefix(ERR_BADCHANNAME(client.getNickName()));
    else {
        //your code
    }
}

void Server::kick(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::part(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::notice(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::privmsg(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    //else if (arguments.size() >= 2 && arguments[1][0] == ':') client.ServerToClientPrefix(ERR_NORECIPIENT (client.getNickName()));
}

void Server::quit(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::topic(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::names(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::list(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::invite(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::mode(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::oper(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::wallops(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}

void Server::whois(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}
