#include "Server.hpp"

Server::Server() : serverSocket(0), port(0), maxNumOfClients(0) {
    
}

Server::Server(std::string ip, int port, std::string password, size_t num) : ip(ip), port(port), password(password), maxNumOfClients(num) {}

Server::Server(const Server& copy) {*this = copy;}

Server::~Server() {}

Server& Server::operator=(const Server& right) {
    if (this == &right) {
        return *this;
    }

    this->serverSocket = right.serverSocket;
    this->ip = right.ip;
    this->port = right.port;
    this->password = right.password;
    this->maxNumOfClients = right.maxNumOfClients;
    this->channels = right.channels;
    this->timeOfCreation = right.timeOfCreation;
    this->cmdMap = right.cmdMap;

    clients.clear();
    std::map<int, Client>::const_iterator it;
    for (it = right.clients.begin(); it != right.clients.end(); ++it) {
        clients[it->first] = it->second;
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
    , "topic", "names", "list", "invite", "mode", "whois"};
    cmd cmd_ptrs[] = {&Server::join, &Server::kick, 
    &Server::part, &Server::notice, &Server::privmsg, 
    &Server::quit, &Server::topic, &Server::names, 
    &Server::list, &Server::invite, &Server::mode, &Server::whois};
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
            } else if (clients.size() >= this->maxNumOfClients) {
                std::cout << "Maximum number of clients reached!" << std::endl;
                continue;
            }

            std::cout << "A client has connected!." << std::endl;
            this->clients[clientSocket] = Client(clientSocket);
            this->clients[clientSocket].setServerHostName(this->ip);
            this->clients[clientSocket].timeJoined = std::time(NULL);

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
        std::map<int, Client>::iterator it = this->clients.find(clientSocket);
        std::cout << "The client " << it->second.getNickName() << " has been disconnected." << std::endl;
        // Remove the client socket from the map
        if (channels.size() > 0) {
            for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ch_it++) {
                if (doesClientExistInChannel(*ch_it, it->second.getNickName())) {
                    ch_it->removeMember(it->second.getNickName());
                    break;
                }
            }
        }

        close(clientSocket);
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
    if (this->password.empty()) client.logged = true;
    if (tolower(trim(arguments[0])) == "pass") {
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
    if (this->password.empty()) {
        client.logged = true;
        return ;
    }
    else if (arguments.size() > 2 || arguments.size() == 1) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
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
            if ((!client.getPassWord().empty() || this->password.empty()) && !client.getNickName().empty() && !client.getUserName().empty()) {
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
        if ((!client.getPassWord().empty() || this->password.empty()) && !client.getNickName().empty() && !client.getUserName().empty()) {
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

void Server::sendMessageToClient(Client& sender, const std::string& message, int receiver) {
    std::string msg = ":" + sender.getPrefixClient() + message + "\r\n";
    if (send(receiver, msg.c_str(), msg.length(), 0) < 0)
        throw std::runtime_error("An error occurred while attempting to send a message to the client.\n");
    msg.clear();
}

void Server::sendToChannelMembers(Channel* Channel, Client& client, std::string msg) {
    size_t i = 0;
    std::string message = ":" + client.getPrefixClient() + msg + "\r\n";
    while (i < Channel->clients.size()) {
        if (send(Channel->clients[i]->getClientSocket(), message.c_str(), message.length(), 0) < 0)
            throw std::runtime_error("An error occurred while attempting to send a message to the client.\n");
        i++;
    }
}

bool Server::doesChannelExist(std::string name) {
    if (channels.size() == 0) return false;
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->getchannelName() == name) {
            return true;
        }
    }
    return false;
}

bool Server::doesClientExistInChannel(Channel& ch, std::string clientName) {
    if (ch.isempty) return false;
    for (std::vector<Client *>::iterator it = ch.clients.begin(); it != ch.clients.end(); ++it) {   
        if ((**it).getNickName() == clientName)
            return true;
    }
    return false;
}

Channel* Server::getChannel(std::string channelName) {
    for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); it++) {
        if (channelName == it->getchannelName())
            return &(*it);
    }
    return NULL;
}

Client* Server::getClient(std::string clientName) {
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (clientName == it->second.getNickName())
            return &(it->second);
    }
    return NULL;
}

bool Server::clientExists(std::string clientName) {
    for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        if (it->second.getNickName() == clientName) return true;
    return false;
}
