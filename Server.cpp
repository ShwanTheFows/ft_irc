#include "Server.hpp"

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

    if (port < 1 || port > 65535) {
        std::cerr << "The port number should be between 1-65,535!" << std::endl;
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

void Server::setUpSocket(void) {
    this->serverSocket = -1;
    this->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->serverSocket == -1) {
        std::cout << "Error in seting up the server socket!" << std::endl;
        throw Server::Error();
    } else {
        std::cout << "The server socket has been set up succefully!" << std::endl;
    }
}

void Server::bind(void) {
    sockaddr_in service;
    service.sin_family = AF_INET;
    if (inet_pton(AF_INET, this->ip.c_str(), &(service.sin_addr)) <= 0 ) {
        std::cout << "Invalid server address!" << std::endl;
        throw Server::Error();
    }
    service.sin_port = htons(this->port);
    if (::bind(this->serverSocket, reinterpret_cast<struct sockaddr*>(&service), sizeof(service)) != 0) {
        std::cout << "Binding failed!" << std::endl;
        throw Server::Error();
    } else {
        std::cout << "Binding is done succefully!" << std::endl;
    }
}

struct ThreadArgs {
    Server* server;
    int clientSocket;
};

// Static wrapper function
static void* handleClientWrapper(void* arg)
{
    ThreadArgs* threadArgs = static_cast<ThreadArgs*>(arg);
    threadArgs->server->handleClient(threadArgs->clientSocket);
    delete threadArgs; // Remember to deallocate the memory
    return NULL;
}

void Server::listen(void) {
    if (::listen(this->serverSocket, 1) == -1) {
        std::cout << "Listening failed!" << std::endl;
        throw Server::Error();
    } else {
        std::cout << "Listening on port "  <<  this->port << "..." << std::endl;
    }

    while (true) {
        int clientSocket = accept(this->serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            std::cout << "Accept failed!" << std::endl;
            throw Server::Error();
        } else {
            std::cout << "Accepted connection." << std::endl;
            this->clientSockets.push_back(clientSocket);
            //std::thread clientThread(&Server::handleClient, this, clientSocket);
            pthread_t clientThread;
            ThreadArgs* threadArgs = new ThreadArgs;
            threadArgs->server = this;
            threadArgs->clientSocket = clientSocket;
            pthread_create(&clientThread, NULL, &handleClientWrapper, threadArgs);
            this->clientThread.detach();
        }
    }
}

void Server::handleClient(int clientSocket) {
    int bufsize = 512;
    char buffer[bufsize];

    while (true) {
        memset(buffer, 0, bufsize);  // Clear the buffer

        // Receive message from the client
        int bytesRead = recv(this->acceptSocket, buffer, bufsize - 1, 0);

        if (bytesRead > 0) {
            // Print the received message
            std::cout << "Received message: " << tolower(buffer) << std::endl;
        } else if (bytesRead == 0) {
            std::cout << "Client disconnected." << std::endl;
            close(clientSocket);
            this->clientSockets.erase(std::remove(this->clientSockets.begin(), this->clientSockets.end(), clientSocket), this->clientSockets.end());
            break;  // Client disconnected, exit the loop
        } else {
            std::cout << "Error in receiving message." << std::endl;
            break;  // Error occurred, exit the loop
        }
    }
}

// void Server::receiver() {
//     int bufsize = 512;
//     char buffer[bufsize];

//     while (true) {
//         memset(buffer, 0, bufsize);  // Clear the buffer

//         // Receive message from the client
//         int bytesRead = recv(this->acceptSocket, buffer, bufsize - 1, 0);

//         if (bytesRead > 0) {
//             // Print the received message
//             std::cout << "Received message: " << tolower(buffer) << std::endl;
//         } else if (bytesRead == 0) {
//             std::cout << "Client disconnected." << std::endl;
//             break;  // Client disconnected, exit the loop
//         } else {
//             std::cout << "Error in receiving message." << std::endl;
//             break;  // Error occurred, exit the loop
//         }
//     }
// }

void Server::disconnect(void) {
    close(this->serverSocket);
}