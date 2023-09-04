#include "../Server.hpp"

void Server::notice(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 3) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() >= 3 && trim(arguments[1])[0] == ':') client.ServerToClientPrefix(ERR_NORECIPIENT(client.getNickName(), joinVectorFromIndex(arguments, 0)));
    else if (arguments.size() >= 3 && trim(arguments[2])[0] != ':') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (trim(arguments[1])[0] != '#') {
        for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.getNickName() == trim(arguments[1])) {
                Client clientCopy = it->second;
                sendMessageToClient(clientCopy,  "NOTICE " + it->second.getNickName() + " :" + joinVectorFromIndex(arguments, 2));
                return ;
            }
        }
        client.ServerToClientPrefix(ERR_NOSUCHNICK(trim(arguments[1])));
    } else if (trim(arguments[1])[0] == '#') {
        std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (it->getchannelName() == arguments[1]) {
                 for (std::vector<Client *>::iterator myit = it->clients.begin(); myit != it->clients.end(); ++myit) {   
                        sendMessageToClient(**myit, "NOTICE " + (**myit).getNickName() + " :" + joinVectorFromIndex(arguments, 2));
                }
                return ;
            }
        }
        ///////////////change this
        if (it == channels.end()) {
            client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(),trim(arguments[1])));
        }

    }
}