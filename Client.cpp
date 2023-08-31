#include "Client.hpp"

Client::Client() : hostName("127.0.0.1"), port(55555) {}

Client::Client(int fd) : userName("default"), nickName("default"), hostName("default"), realName("default"), port(12345), clientSocket(fd) {}

Client::Client(const Client& copy) {*this = copy;}

Client::~Client() {}

Client& Client::operator=(const Client& right) {
    if (this != &right) {
        this->userName = right.getUserName();
        this->nickName = right.getNickName();
        this->hostName = right.getHostName();
        this->realName = right.getRealName();
        this->clientSocket = right.getClientSocket();
    }

    return *this;
}

const char* Client::Error::what() const throw() {
    return "Error!";
}

void Client::setUserName(std::string username) {this->userName = username;}

void Client::setNickName(std::string nickname) {this->nickName = nickname;}

void Client::setHostName(std::string hostname) {this->hostName = hostname;}

void Client::setRealName(std::string realname) {this->realName = realname;}

void Client::setClientSocket(int socket) {this->clientSocket = socket;}

void Client::setPassWord(std::string passwd) {this->password = passwd;}

std::string Client::getUserName(void) const {return this->userName;}

std::string Client::getNickName(void) const {return this->nickName;}

std::string Client::getHostName(void) const {return this->hostName;}

std::string Client::getRealName(void) const {return this->realName;}

int Client::getClientSocket(void) const {return this->clientSocket;}

std::string Client::getPassWord(void) const {return this->password;}

void Client::setUpSocket(void) {
    this->clientSocket = -1;
    this->clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->clientSocket == -1) {
        std::cout << "Error in seting up the client socket!" << std::endl;
        throw Client::Error();
    } else {
        std::cout << "The client socket has been set up succefully!" << std::endl;
    }
}

void Client::connect() {
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    if (inet_pton(AF_INET, this->hostName.c_str(), &(clientService.sin_addr)) <= 0 ) {
        std::cout << "Invalid client address!" << std::endl;
        throw Client::Error();
    }
    clientService.sin_port = htons(this->port);
    if (::connect(this->clientSocket, reinterpret_cast<struct sockaddr*>(&clientService), sizeof(clientService)) != 0) {
        std::cout << "Client failed to connect!" << std::endl;
        throw Client::Error();
    } else {
        std::cout << "Client has connected succesfully!" << std::endl;
    }
}

void Client::messaging(void) {
    bool isExit = false;
    int bufsize = 512;
    char buffer[bufsize];

    recv(this->clientSocket, buffer, bufsize, 0);

    do {
        std::cout << "Client: ";
        do {
            std::cin >> buffer;
            send(this->clientSocket, buffer, bufsize, 0);
            if (*buffer == '#') {
                send(this->clientSocket, buffer, bufsize, 0);
                *buffer = '*';
                isExit = true;
            }
        } while (*buffer != 42);

        std::cout << "Server: ";
        do {
            recv(this->clientSocket, buffer, bufsize, 0);
            std::cout << buffer << " ";
            if (*buffer == '#') {
                *buffer = '*';
                isExit = true;
            }

        } while (*buffer != 42);
        std::cout << std::endl;

    } while (!isExit);

}

void Client::disconnect(void) {
    close(this->clientSocket);
}