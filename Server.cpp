#include "Server.hpp"

Server::Server() : serverSocket(0), port(0) {
    
}

Server::Server(std::string ip, int port, std::string password) : ip(ip), port(port), password(password) {}

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
    this->channels = right.channels;
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
    , "topic", "names", "list", "invite", "mode", "whois", "pong"};
    cmd cmd_ptrs[] = {&Server::join, &Server::kick, 
    &Server::part, &Server::notice, &Server::privmsg, 
    &Server::quit, &Server::topic, &Server::names, 
    &Server::list, &Server::invite, &Server::mode, &Server::whois, &Server::pong};
    int num_cmds = sizeof(cmd_ptrs) / sizeof(cmd);

    for (int i = 0; i < num_cmds; i++)
        cmdMap.insert(std::make_pair(cmd_strings[i], cmd_ptrs[i]));
}

void Server::run() {
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

            std::cout << "A client is connected!." << std::endl;
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
    int	val = 1;
    this->serverSocket = -1;
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket < 0) {
        throw std::runtime_error("Server socket setup failed.");
    }
    if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR,  &val, sizeof(val)) < 0)
		throw std::runtime_error("Error while setting socket options");
	if (fcntl(this->serverSocket, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error while setting server's file socket to non-blocking");
    std::cout << "The server socket has been set up successfully!" << std::endl;
}

void Server::bind() {
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(this->port);
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(this->serverSocket, reinterpret_cast<struct sockaddr*>(&service), sizeof(service)) < 0) {
        throw std::runtime_error("Binding failed.");
    } else {
        std::cout << "Binding is done successfully!" << std::endl;
    }
}

void Server::listen() {
    if (::listen(this->serverSocket, 1) < 0) {
        throw std::runtime_error("Listening failed.");
    } else {
        std::cout << "Listening for clients..." << std::endl;
    }
}

void Server::handleClient(int clientSocket) {
    int buffsize = 512;
    char buffer[buffsize];

    memset(buffer, 0, buffsize);

    int bytesRead = recv(clientSocket, buffer, buffsize, 0);

    if (bytesRead > 0) {
        if (bytesRead >= buffsize) {
            buffer[buffsize] = '\0';
        }
        if (buffer[strlen(buffer) - 1] != '\n' && buffer[strlen(buffer) - 1] != '\r') {
            clients[clientSocket]._buffer += buffer;
        } else {
            clients[clientSocket]._buffer += buffer;
            std::vector<std::string> args = splitStringByNewline(clients[clientSocket]._buffer);
            for (size_t i = 0; i < args.size(); i++) {
                removeTrailingNewline(args[i]);
                if (!this->checkLoginCommands(clients[clientSocket], args[i])) {
                    if (this->clients[clientSocket].isRegistered && !this->checkCommands(clients[clientSocket], args[i]))
                        clients[clientSocket].ServerToClientPrefix(ERR_UNKNOWNCOMMAND(clients[clientSocket].getNickName(), args[i]));
                }
            }
            clients[clientSocket]._buffer = "";
        }
    } else if (bytesRead == 0) {
        std::map<int, Client>::iterator it = this->clients.find(clientSocket);
        std::cout << "A client is disconnected." << std::endl;
        // Remove the client socket from the map
        if (channels.size() > 0) {
            for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ch_it++) {
                if (doesClientExistInChannel(*ch_it, it->second.getNickName())) {
                    ch_it->removeMember(it->second.getNickName());
                    if (ch_it->clients.size() == 0) {
                        removeChannel(ch_it->getchannelName());
                    }
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
    else if (!client.isRegistered) {
        client.ServerToClientPrefix(ERR_UNKNOWNCOMMAND(client.getNickName(), buffer));
    }
    return false;
}

void Server::checkPassword(Client& client, std::vector<std::string>& arguments) {
    if (this->password.empty()) {
        client.logged = true;
        return ;
    }
    else if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
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
    if (arguments.size() != 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (trim(arguments[1]).empty()) client.ServerToClientPrefix(ERR_NONICKNAMEGIVEN(client.getNickName()));
    else {
        if (!isValidNickname(trim(arguments[1]))) client.ServerToClientPrefix(ERR_ERRONEUSNICKNAME(client.getNickName()));
        else {
            for (std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); it++) {
                if (tolower(it->second.getNickName()) == tolower(trim(arguments[1]))) {
                    client.ServerToClientPrefix(ERR_NICKNAMEINUSE(client.getNickName()));
                    return ;
                }
            }
            if (!client.getNickName().empty()) {
                for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); it++)
                    it->changeNickname(client.getNickName(), trim(arguments[1]));
            }
            client.setNickName(trim(arguments[1]));
            if ((!client.getPassWord().empty() || this->password.empty()) && !client.getNickName().empty() && !client.getUserName().empty() && !client.isRegistered) {
                client.isRegistered = true;
                client.welcome();
            }
        }
    }
}

void Server::checkUserCommand(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 5) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (trim(arguments[1])[0] == ':' || trim(arguments[2])[0] == ':' || trim(arguments[3])[0] == ':') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (client.isRegistered) client.ServerToClientPrefix(ERR_ALREADYREGISTRED(client.getNickName()));
    else if (client.getUserName().empty()) {
        if (!isValidUsername(trim(arguments[1]))) client.ServerToClientPrefix(ERR_ERRONEUSNICKNAME(client.getNickName()));
        else {
            client.setUserName(trim(arguments[1]));
            if (trim(arguments[4])[0] == ':')
                client.setRealName(joinVectorFromIndex(arguments, 4));
            else
                client.setRealName(trim(arguments[4]));
            if ((!client.getPassWord().empty() || this->password.empty()) && !client.getNickName().empty() && !client.getUserName().empty()) {
                client.isRegistered = true;
                client.welcome();
            }
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

void Server::sendToChannelMembers(Channel* channel, Client& client, std::string msg) {
    size_t i = 0;
    std::string message = ":" + client.getPrefixClient() + msg + "\r\n";
    while (i < channel->clients.size()) {
        if (send(channel->clients[i]->getClientSocket(), message.c_str(), message.length(), 0) < 0)
            throw std::runtime_error("An error occurred while attempting to send a message to the client.\n");
        i++;
    }
}

void Server::sendToChannelMembersExceptClient(Channel* channel, Client& client, std::string msg) {
    size_t i = 0;
    std::string message = ":" + client.getPrefixClient() + msg + "\r\n";
    while (i < channel->clients.size()) {
        if (client.getClientSocket() != channel->clients[i]->getClientSocket()) {
            if (send(channel->clients[i]->getClientSocket(), message.c_str(), message.length(), 0) < 0)
                throw std::runtime_error("An error occurred while attempting to send a message to the client.\n");
        }
        i++;
    }
}

bool Server::doesChannelExist(std::string name) {
    if (channels.size() == 0) return false;
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (tolower(it->getchannelName()) == tolower(name)) {
            return true;
        }
    }
    return false;
}

bool Server::doesClientExistInChannel(Channel& ch, std::string clientName) {
    if (ch.isempty) return false;
    for (std::vector<Client *>::iterator it = ch.clients.begin(); it != ch.clients.end(); ++it) {
        if (tolower((**it).getNickName()) == tolower(clientName))
            return true;
    }
    return false;
}

Channel* Server::getChannel(std::string channelName) {
    for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); it++) {
        if (tolower(channelName) == tolower(it->getchannelName()))
            return &(*it);
    }
    return NULL;
}

Client* Server::getClient(std::string clientName) {
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (tolower(clientName) == tolower(it->second.getNickName()))
            return &(it->second);
    }
    return NULL;
}

bool Server::clientExists(std::string clientName) {
    for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        if (tolower(it->second.getNickName()) == tolower(clientName)) return true;
    return false;
}

void Server::removeChannel(std::string channelName) {
    if (channels.size() == 0) return ;
    for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); it++) {
        if (tolower(channelName) == tolower(it->getchannelName())) {
            channels.erase(it);
            return ;
        }
    }
}

void Server::pong(Client& client, std::vector<std::string>& arguments) {(void)client; (void)arguments;}
