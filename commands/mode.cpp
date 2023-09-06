#include "../Server.hpp"

void Server::mode(Client& client, std::vector<std::string>& arguments) {
    channel* ch;
    if (arguments.size() >= 2)
        ch = getChannel(trim(arguments[1]));
    if (arguments.size() < 3 || arguments.size() > 4) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (trim(arguments[2]).length() != 2) client.ServerToClientPrefix(ERR_UNKNOWNMODE(client.getNickName(), joinVectorFromIndex(arguments, 2)));
    else if (arguments.size() > 3 && trim(arguments[2])[0] == '-' && tolower(trim(arguments[2])[1]) != 'o') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 3 && trim(arguments[2])[0] == '+' && tolower(trim(arguments[2])[1]) != 'i') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (!doesChannelExist(trim(arguments[1]))) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
    else if (!ch->isOp(client.getNickName())) client.ServerToClientPrefix(ERR_CHANOPRIVSNEEDED(client.getNickName(), trim(arguments[1])));
    else if (arguments.size() == 3 && trim(arguments[2])[0] == '-') {
        std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (it->getchannelName() == trim(arguments[1])) {  
                if (tolower(trim(arguments[2])[1]) == 'i')
                    it->setPrivate(false);
                else if (tolower(trim(arguments[2])[1]) == 't')
                    it->boolTopic(false);
                else if (tolower(trim(arguments[2])[1]) == 'k')
                    it->haveKey(false);
                else if (tolower(trim(arguments[2])[1]) == 'l')
                    it->haveLimit(false);
                else if (tolower(trim(arguments[2])[1]) == 'o')
                    client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
                else
                    client.ServerToClientPrefix(ERR_UNKNOWNMODE(client.getNickName(), joinVectorFromIndex(arguments, 2)));
            }
        }        
    }
    else if (arguments.size() == 4 && trim(arguments[2])[0] == '-') {
        std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (tolower(trim(arguments[2])[1]) == 'o'){
                if (it->getchannelName() == trim(arguments[1])) { 
                    std::vector<Client *>::iterator myit = it->clients.begin();
                    for (; myit != it->clients.end(); ++myit) {   
                        if ((*myit)->getNickName() == trim(arguments[3])) {
                            it->removeOp((*myit)->getNickName());
                            return ;
                        }
                    }
                    client.ServerToClientPrefix(ERR_NOSUCHNICK(trim(arguments[3])));
                }
            } else client.ServerToClientPrefix(ERR_UNKNOWNMODE(client.getNickName(), joinVectorFromIndex(arguments, 2)));
        }
    }
    else if (arguments.size() >= 3 && trim(arguments[2])[0] == '+') {
        std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (it->getchannelName() == trim(arguments[1])) {
                if (tolower(trim(arguments[2])[1]) == 'i')
                    it->setPrivate(true);
                else if (tolower(trim(arguments[2])[1]) == 't') {
                    it->boolTopic(true);
                    it->setTopic(trim(arguments[3]));
                }
                else if (tolower(trim(arguments[2])[1]) == 'k') {
                    it->haveKey(true);
                    it->setKey(trim(arguments[3]));
                }
                else if (tolower(trim(arguments[2])[1]) == 'o') {
                    std::vector<Client *>::iterator myit = it->clients.begin();
                    for (; myit != it->clients.end(); ++myit) {
                        if ((*myit)->getNickName() == trim(arguments[3])) {
                            it->setOperators((*myit)->getNickName());
                            return ;
                        }
                    }
                    client.ServerToClientPrefix(ERR_NOSUCHNICK(trim(arguments[3])));
                }
                else if (tolower(trim(arguments[2])[1]) == 'l') {
                    int limit = std::atoi(trim(arguments[3]).c_str());
                    if (limit < 0) client.ServerToClientPrefix(ERR_UNKNOWNMODE(client.getNickName(), joinVectorFromIndex(arguments, 2)));
                    else {
                        it->haveLimit(true);
                        it->setLimit(limit);
                    }
                }
                else
                    client.ServerToClientPrefix(ERR_UNKNOWNMODE(client.getNickName(), joinVectorFromIndex(arguments, 2)));
            }
        }
    } else client.ServerToClientPrefix(ERR_UNKNOWNMODE(client.getNickName(), joinVectorFromIndex(arguments, 2)));
}