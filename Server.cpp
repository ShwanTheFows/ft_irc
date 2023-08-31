#include "Server.hpp"

std::vector<std::string> splitString(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream inputStringStream(str);
    std::string token;
    while (inputStringStream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string tolower(const std::string& input) {
    std::string result = input;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::tolower(result[i]);
    }
    return result;
}

int parsing(const std::string& str) {
    int port;

    if (str.empty()) {
        std::cerr << "Please enter port number!" << std::endl;
        exit(1);
    }

    size_t i = 0;

    if (str[i] == '+' || str[i] == '-')
        ++i;

    for (; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            std::cerr << "The port should be a number!" << std::endl;
            exit(1);
        }
    }

    port = std::atoi(str.c_str());

    if (port < 1024 || port > 65535) {
        std::cerr << "Invalid port number!" << std::endl;
        exit(1);
    }

    return port;
}

Server::Server() : ip("127.0.0.1"), port(55555) {}

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

void Server::run() {
    setUpSocket();
    bind();
    listen();

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
    int bytesRead = recv(clientSocket, buffer, bufsize - 1, 0);

    if (bytesRead > 0) {
        // Print the received message
        if (!this->checkCommand(clients[clientSocket], buffer)) {
            if (this->clients[clientSocket].getPassWord() == this->password) {
                std::cout << "Received message: " << buffer << std::endl;

                // // Send the received message to all other clients
                // for (size_t i = 0; i < this->clients.size(); ++i) {
                //     if (this->clients[i].clientSocket != clientSocket) {
                //         send(this->clients[i].clientSocket, buffer, bytesRead, 0);
                //     }
                // }
            }
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

bool Server::checkCommand(Client& client, std::string buffer) {
    std::vector<std::string> arguments = splitString(buffer);
    if (tolower(arguments[0]) == "pass") {
        checkPassword(client, arguments);
        return true;
    } //else if JOIN...
    else return false;
}

void Server::checkPassword(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() > 2)client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (client.getPassWord().length() == 0 && this->password.length() != 0) {
        if (arguments[1] != this->password) client.ServerToClientPrefix(ERR_PASSWDMISMATCH(client.getNickName()));
        else if (arguments[1] == this->password) client.setPassWord(arguments[1]);
    }
}

void Server::disconnect() {
    close(this->serverSocket);
    for (std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
        close(it->second.getClientSocket());
    this->clients.clear();
}