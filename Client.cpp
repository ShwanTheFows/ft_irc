#include "Client.hpp"

Client::Client() {}

Client::Client(int fd) : _buffer(""), clientSocket(fd), logged(false), isRegistered(false){
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
        this->password = right.getPassWord();
        this->clientSocket = right.getClientSocket();
        this->serverHostName = right.getServerHostName();
        this->timeJoined = right.timeJoined;
        this->logged = right.logged;
        this->isRegistered = right.isRegistered;
        this->_buffer = right._buffer;
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


const std::string Client::getJoiningTime(void) const {
    std::stringstream ss;
    ss << this->timeJoined;
    return ss.str();
}

std::string Client::getPrefixClient(void) const {
    return this->getNickName() + "!" + this->getUserName() + "@" + this->getHostName() + " ";
}

void Client::ClientToClientPrefix(std::string str) {
    mySend(":" + getPrefixClient() + str);
}

void Client::ServerToClientPrefix(std::string str) {
    mySend(":" + getServerHostName() + " " + str);
}

void Client::mySend(std::string str) {
    std::string buff = str + "\r\n";
    if (send(this->clientSocket, buff.c_str(), buff.length(), 0) < 0)
        throw std::runtime_error("An error occurred while attempting to send a message to the client.\n");
}

void Client::welcome() {
    if (!this->nickName.empty() && !this->userName.empty())
    {
        std::cout << this->nickName << " has registered!" << std::endl;
        isRegistered = true;
        ServerToClientPrefix(RPL_WELCOME(this->nickName, "IRC", this->userName, this->hostName));
        ServerToClientPrefix(RPL_YOURHOST(this->nickName, this->hostName));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "          _          _            _            _            _            _     _          _ "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "         /\\ \\       /\\ \\        /\\ \\          / /\\         /\\ \\         /\\ \\  /\\ \\    _ / /\\  "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "         \\ \\ \\     /  \\ \\      /  \\ \\        / /  \\       /  \\ \\       /  \\ \\ \\ \\ \\  /_/ / /  "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "         /\\ \\_\\   / /\\ \\ \\    / /\\ \\ \\      / / /\\ \\__   / /\\ \\ \\     / /\\ \\ \\ \\ \\ \\ \\___\\/   "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "        / /\\/_/  / / /\\ \\_\\  / / /\\ \\ \\    / / /\\ \\___\\ / / /\\ \\_\\   / / /\\ \\_\\/ / /  \\ \\ \\   "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "       / / /    / / /_/ / / / / /  \\ \\_\\   \\ \\ \\ \\/___// /_/_ \\/_/  / / /_/ / /\\ \\ \\   \\_\\ \\  "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "      / / /    / / /__\\/ / / / /    \\/_/    \\ \\ \\     / /____/\\    / / /__\\/ /  \\ \\ \\  / / /  "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "     / / /    / / /_____/ / / /         _    \\ \\ \\   / /\\____\\/   / / /_____/    \\ \\ \\/ / /   "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, " ___/ / /__  / / /\\ \\ \\  / / /________ /_/\\__/ / /  / / /______  / / /\\ \\ \\       \\ \\ \\/ /    "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "/\\__\\/_/___\\/ / /  \\ \\ \\/ / /_________\\\\ \\/___/ /  / / /_______\\/ / /  \\ \\ \\       \\ \\  /     "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, "\\/_________/\\/_/    \\_\\/\\/____________/ \\_____\\/   \\/__________/\\/_/    \\_\\/        \\_\\/      "));
        ServerToClientPrefix(RPL_MOTD(this->nickName, " "));
        ServerToClientPrefix(RPL_ENDOFMOTD(this->nickName));
    }
}
