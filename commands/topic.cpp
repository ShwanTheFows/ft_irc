#include "../Server.hpp"


void Server::topic(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() > 2 && trim(arguments[2])[0] != ':') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 2) {
        std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (it->getchannelName() == arguments[1]) {
                std::cout << it->getTopic() << std::endl;
                return ;
            }
        }
        if (it == channels.end()) {
            client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(),trim(arguments[1])));
        }
    } else if (arguments.size() > 2) {
      std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (it->getchannelName() == arguments[1]) {
                it->setTopic(joinVectorFromIndex(arguments,2));
                return ;
            }
        }
        if (it == channels.end()) {
            client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(),trim(arguments[1])));
        }  
    }
}