#include "Client.hpp"

Client::Client(){}

Client::Client(int fd) : clientSocket(fd), logged(false), isRegistered(false)  {
    this->setHostName(getHostIpAddress());
}

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

void Client::setServerHostName(std::string hostname) {this->serverHostName = hostname;}

std::string Client::getUserName(void) const {return this->userName;}

std::string Client::getNickName(void) const {return this->nickName;}

std::string Client::getHostName(void) const {return this->hostName;}

std::string Client::getRealName(void) const {return this->realName;}

int Client::getClientSocket(void) const {return this->clientSocket;}

std::string Client::getPassWord(void) const {return this->password;}

std::string Client::getServerHostName(void) const {return this->serverHostName;}

std::string Client::getPrefixClient(void) const {
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

void Client::welcome(std::string timeOfCreation) {
    //if (isGuest && !getNickName().empty() && !getRealName().empty() && !getUserName().empty())
    if (!this->nickName.empty() && !this->userName.empty())
    {
        std::cout << this->nickName << " has registered!" << std::endl;
        //isGuest = false;
        isRegistered = true;
        ServerToClientPrefix(RPL_WELCOME(this->nickName, "IRC", this->userName, this->hostName));
        ServerToClientPrefix(RPL_YOURHOST(this->nickName, this->hostName));
        ServerToClientPrefix(RPL_CREATED(this->nickName, timeOfCreation));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "- " + this->serverHostName + " Message of the day"));
        ServerToClientPrefix(RPL_MOTD(this->nickName, " ___________  _____  _____ ___________ _   _ "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "|_   _| ___ \\/  __ \\/  ___|  ___| ___ \\ | | |"));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "  | | | |_/ /| /  \\/\\ `--.| |__ | |_/ / | | |"));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "  | | |    / | |     `--. \\  __||    /| | | |"));
        ServerToClientPrefix(RPL_MOTD(this->nickName, " _| |_| |\\ \\ | \\__/\\/\\__/ / |___| |\\ \\\\ \\_/ /"));
        ServerToClientPrefix(RPL_MOTD(this->nickName, " \\___/\\_| \\_| \\____/\\____/\\____/\\_| \\_|\\___/ "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, " "));
        ServerToClientPrefix(RPL_ENDOFMOTD(this->nickName));
        ServerToClientPrefix(RPL_UMODEIS(this->nickName, "+w"));
    }
}