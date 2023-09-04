#include "../Server.hpp"

void Server::mode(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 3 || arguments.size() > 4) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() > 3 && trim(arguments[2])[0] == '-') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 3 && trim(arguments[2])[0] == '+') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 3 && trim(arguments[2])[0] == '-') {
        std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (it->getchannelName() == arguments[1]) {  
                if (tolower(trim(arguments[2])[1]) == 'i')
                {
                    it->setPrivate(false);
                }
                else if (tolower(trim(arguments[2])[1]) == 't')
                {
                    it->boolTopic(false);
                }
                else if (tolower(trim(arguments[2])[1]) == 'k')
                {
                    it->haveKey(false);
                }
                
                else if (tolower(trim(arguments[2])[1]) == 'l')
                {
                    it->haveLimit(false);
                }
            }
        }        
    }
    else if (arguments.size() == 4 && trim(arguments[2])[0] == '-') {
        std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (tolower(trim(arguments[2])[1]) == 'o'){
                if (it->getchannelName() == arguments[1]) { 
                    std::vector<Client *>::iterator myit = it->clients.begin();
                    for (; myit != it->clients.end(); ++myit) {   
                        if ((*myit)->getNickName() == trim(arguments[3]))
                        {
                            (*myit)->setOp(false);
                            break;
                        }
                    }
                    if (myit == it->clients.end())
                        client.ServerToClientPrefix(ERR_NOSUCHNICK(arguments[3]));
                }
            }
        }
    }
    else if (arguments.size() == 4 && trim(arguments[2])[0] == '+') {
        std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (it->getchannelName() == arguments[1]) {
                if (tolower(trim(arguments[2])[1]) == 'i')
                {
                    it->setPrivate(true);
                }
                else if (tolower(trim(arguments[2])[1]) == 't')
                {
                    it->boolTopic(true);
                    it->setTopic(trim(arguments[3]));
                }
                else if (tolower(trim(arguments[2])[1]) == 'k')
                {
                    it->haveKey(true);
                    it->setKey(trim(arguments[3]));
                }
                else if (tolower(trim(arguments[2])[1]) == 'o')
                {
                    std::vector<Client *>::iterator myit = it->clients.begin();
                    for (; myit != it->clients.end(); ++myit) {
                        if ((*myit)->getNickName() == trim(arguments[3]))
                        {
                            (*myit)->setOp(true);
                            break;
                        }
                    }
                    if (myit == it->clients.end())
                        client.ServerToClientPrefix(ERR_NOSUCHNICK(arguments[3]));
                }
                else if (tolower(trim(arguments[2])[1]) == 'l')
                {
                    it->haveLimit(false);
                }
            }
        }
    }
    
}