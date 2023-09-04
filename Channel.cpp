#include "Channel.hpp"

channel::channel(std::string name, Client& member) : _name(name)
{
    member.setOp(true);
    haveKey(false);
    clients.push_back(&member);
}
channel::channel(std::string name, Client& member, std::string key) : _name(name)
{
    member.setOp(true);
    haveKey(true);
    setKey(key);
    clients.push_back(&member);
}

std::string channel::getchannelName(void)
{
    return(this->_name);
}

void channel::addMember(Client& member)
{
        std::vector<Client *>::iterator c_it;
        for (c_it = clients.begin(); c_it != clients.end(); ++c_it) {
            if ((*c_it)->getNickName() == member.getNickName()) {
                member.ServerToClientPrefix(ERR_USERONCHANNEL(member.getNickName(), getchannelName()));
                break;
            }
        }
         if (c_it == clients.end()) {
            clients.push_back(&member);
         }
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
        if ((**myit).isOperator()) result += '@' + (**myit).getNickName();
        else result += (**myit).getNickName();
        result += ' ';
    }
    return result;
}