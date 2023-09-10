#include "Channel.hpp"

Channel::Channel() {this->isempty = true;}

Channel::Channel(std::string name, Client& member) : _name(name), isempty(false)
{
    this->isPrivate = false;
    haveKey(false);
    clients.push_back(&member);
    operators.push_back(member.getNickName());
}
Channel::Channel(std::string name, Client& member, std::string key) : _name(name), _key(key), isempty(false)
{
    this->isPrivate = false;
    haveKey(true);
    clients.push_back(&member);
    operators.push_back(member.getNickName());
}

Channel::Channel(const Channel& other) {*this = other;}

Channel& Channel::operator=(const Channel& other) {
    if (this != &other) {
        _name = other._name;
        _topic = other._topic;
        havekey = other.havekey;
        _key = other._key;
        userLimit = other.userLimit;
        Ulimit = other.Ulimit;
        isPrivate = other.isPrivate;
        haveTopic = other.haveTopic;
        isempty = other.isempty;

        clients.clear();
        std::vector<Client*>::const_iterator itClient;
        for (itClient = other.clients.begin(); itClient != other.clients.end(); ++itClient) {
            clients.push_back(*itClient);
        }

        operators.clear();
        std::vector<std::string>::const_iterator itOp;
        for (itOp = other.operators.begin(); itOp != other.operators.end(); ++itOp) {
            operators.push_back(*itOp);
        }

        inviteList.clear();
        std::vector<std::string>::const_iterator itInvite;
        for (itInvite = other.inviteList.begin(); itInvite != other.inviteList.end(); ++itInvite) {
            inviteList.push_back(*itInvite);
        }
    }
    return *this;
}

Channel::~Channel() {}
std::string Channel::getchannelName(void)
{
    return(this->_name);
}

bool Channel::addMember(Client& member)
{
    if (this->Ulimit && (int)clients.size() >= this->userLimit && !isInInviteList(member.getNickName())) {
        member.ServerToClientPrefix(ERR_CHANNELISFULL(member.getNickName(), getchannelName()));
        return false;
    }
    std::vector<Client *>::iterator c_it;
    for (c_it = clients.begin(); c_it != clients.end(); ++c_it) {
        if (tolower((*c_it)->getNickName()) == tolower(member.getNickName())) {
            member.ServerToClientPrefix(ERR_USERONCHANNEL(member.getNickName(), getchannelName()));
            return false;
        }
    }
    if (c_it == clients.end()) {
        clients.push_back(&member);
    }
    return true;
}

std::string Channel::getTopic(){
    return(this->_topic);
}

void Channel::setTopic(std::string subj)
{
    this->_topic = subj;
}

void Channel::setKey(std::string key){
    this->_key = key;
}

std::string Channel::getKey(void){
    return(this->_key);
}

bool Channel::sethaveKey(){
    return(this->havekey);
}

void Channel::haveKey(bool keyVal){
    this->havekey = keyVal;
}

bool Channel::getPrivate(){
    return(this->isPrivate);
}
void Channel::setPrivate(bool prv)
{
    this->isPrivate = prv;
}

std::string Channel::getClientNames(void) {
    std::string result = "";
    for (std::vector<Client *>::iterator myit = this->clients.begin(); myit != this->clients.end(); ++myit) {
        if (this->isOp((**myit).getNickName())) result += '@' + (**myit).getNickName();
        else result += (**myit).getNickName();
        result += ' ';
    }
    return result;
}

void Channel::removeMember(std::string clientName) {
    for (std::vector<Client *>::iterator myit = this->clients.begin(); myit != this->clients.end(); ++myit) {
        if (tolower((**myit).getNickName()) == tolower(clientName)) {
            if (this->isOp((**myit).getNickName()))
                removeOp((**myit).getNickName());
            if (this->isInInviteList((**myit).getNickName()))
                removeFromInviteList((**myit).getNickName());
            clients.erase(myit);
            return ;
        }
    }
}

void Channel::boolTopic(bool settop){
    this->haveTopic = settop;
}

void Channel::haveLimit(bool change){
    this->Ulimit = change;
}

 int Channel::getLimit()
 {
    return(this->userLimit);
 }
void Channel::setLimit(int lim){
    this->userLimit = lim;
}
bool Channel::hasTopic()
{
    return(this->haveTopic);
}

void Channel::removeOp(std::string name)
{
    if (operators.size() == 0) return ;
    std::vector<std::string>::iterator it = operators.begin();
    for(;it != operators.end();it++) {
        if (tolower(name) == tolower(*it)) {
            operators.erase(it);
            return ;
        }
    }
}

void Channel::setOperators(std::string newOperators) {
    if (this->isOp(newOperators)) return ;
    else operators.push_back(newOperators);
}

bool Channel::isOp(std::string name){
    if (operators.size() == 0) return false;
    std::vector<std::string>::iterator it = operators.begin();
    for(;it != operators.end();it++)
        if (tolower(name) == tolower(*it))
            return true;
    if(it == operators.end())
        return false;
return false;
}

std::string Channel::getClientsSize(void) {
    std::stringstream ss;
    ss << this->clients.size();
    return ss.str();
}

void Channel::addToInviteList(std::string clientName) {
    if (this->inviteList.size() == 0) this->inviteList.push_back(clientName);
    else {
        for (std::vector<std::string>::iterator it = this->inviteList.begin(); it != this->inviteList.end(); it++) {
            if (tolower(*it) == tolower(clientName)) return ;
        }
        this->inviteList.push_back(clientName);
    }
}

void Channel::removeFromInviteList(std::string clientName) {
    if (this->inviteList.size() == 0) return ;
    for (std::vector<std::string>::iterator it = this->inviteList.begin(); it != this->inviteList.end(); it++) {
        if (tolower(*it) == tolower(clientName)) {
            this->inviteList.erase(it);
            return ;
        }
    }
}

bool Channel::isInInviteList(std::string clientName) {
    if (this->inviteList.size() == 0) return false;
    for (std::vector<std::string>::iterator it = this->inviteList.begin(); it != this->inviteList.end(); it++) {
        if (tolower(*it) == tolower(clientName)) return true;
    }
    return false;
}

void Channel::changeNickname(std::string oldNick, std::string newNick) {
    if (isInInviteList(oldNick)) {
        for (std::vector<std::string>::size_type i = 0; i < this->inviteList.size(); i++)
            if (tolower(this->inviteList[i]) == tolower(oldNick))
                this->inviteList[i] = newNick;
    }
    if (isOp(oldNick)) {
        for (std::vector<std::string>::size_type i = 0; i < this->operators.size(); i++)
            if (tolower(this->operators[i]) == tolower(oldNick))
                this->operators[i] = newNick;
    }
}
