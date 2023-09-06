#include "Channel.hpp"

channel::channel() {this->isempty = true;}

channel::channel(std::string name, Client& member) : _name(name), isempty(false)
{
    member.setOp(true);
    this->isPrivate = false;
    haveKey(false);
    clients.push_back(&member);
    operators.push_back(member.getNickName());
}
channel::channel(std::string name, Client& member, std::string key) : _name(name), _key(key), isempty(false)
{
    member.setOp(true);
    this->isPrivate = false;
    haveKey(true);
    clients.push_back(&member);
    operators.push_back(member.getNickName());
}

std::string channel::getchannelName(void)
{
    return(this->_name);
}

bool channel::addMember(Client& member)
{
    std::vector<Client *>::iterator c_it;
    for (c_it = clients.begin(); c_it != clients.end(); ++c_it) {
        if ((*c_it)->getNickName() == member.getNickName()) {
            member.ServerToClientPrefix(ERR_USERONCHANNEL(member.getNickName(), getchannelName()));
            return false;
        }
    }
    if (c_it == clients.end()) {
        clients.push_back(&member);
    }
    return true;
}

std::string channel::getTopic(){
    return(this->_topic);
}

void channel::setTopic(std::string subj)
{
    this->_topic = subj;
}

void channel::setKey(std::string key){
    this->_key = key;
}

std::string channel::getKey(void){
    return(this->_key);
}

bool channel::sethaveKey(){
    return(this->havekey);
}

void channel::haveKey(bool keyVal){
    this->havekey = keyVal;
}

bool channel::getPrivate(){
    return(this->isPrivate);
}
void channel::setPrivate(bool prv)
{
    this->isPrivate = prv;
}

std::string channel::getClientNames(void) {
    std::string result = "";
    for (std::vector<Client *>::iterator myit = this->clients.begin(); myit != this->clients.end(); ++myit) {
        if (this->isOp((**myit).getNickName())) result += '@' + (**myit).getNickName();
        else result += (**myit).getNickName();
        result += ' ';
    }
    return result;
}

void channel::removeMember(std::string clientName) {
    for (std::vector<Client *>::iterator myit = this->clients.begin(); myit != this->clients.end(); ++myit) {
        if ((**myit).getNickName() == clientName) {
            clients.erase(myit);
            return ;
        }
    }
}

void channel::boolTopic(bool settop){
    this->haveTopic = settop;
}

void channel::haveLimit(bool change){
    this->Ulimit = change;
}

 int channel::getLimit()
 {
    return(this->userLimit);
 }
void channel::setLimit(int lim){
    this->userLimit = lim;
}
bool channel::hasTopic()
{
    return(this->haveTopic);
}

void channel::removeOp(std::string name)
{
    if (operators.size() == 0) return ;
    std::vector<std::string>::iterator it = operators.begin();
    for(;it != operators.end();it++)
        if (name == *it)
            operators.erase(it);
}

void channel::setOperators(std::string newOperators) {
    if (this->isOp(newOperators)) return ;
    else operators.push_back(newOperators);
}

bool channel::isOp(std::string name){
    if (operators.size() == 0) return false;
    std::vector<std::string>::iterator it = operators.begin();
    for(;it != operators.end();it++)
        if (name == *it)
            return true;
    if(it == operators.end())
        return false;
return false;
}
