#include "Client.hpp"

Client::Client(){}

Client::Client(int fd, std::string hostname) : hostName(hostname), clientSocket(fd), logged(false) {}

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

std::string Client::getPrefixClient(void) {
    return this->getNickName() + "!" + this->getUserName() + "@" + this->getHostName() + " ";
}

void Client::ClientToClientPrefix(std::string str) {
    mySend(":" + getPrefixClient() + str);
}

void Client::ServerToClientPrefix(std::string str) {
    mySend(":" + getHostName() + " " + str);
}

void Client::mySend(std::string str) {
    std::string buff = str + "\r\n";
    if (send(this->clientSocket, buff.c_str(), buff.length(), 0) < 0)
        throw std::runtime_error("An error occurred while attempting to send a message to the client.\n");
}