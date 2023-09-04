#include "../Server.hpp"

void Server::list(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() > 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 1) {
        std::vector<channel>::iterator it;
        client.ServerToClientPrefix(RPL_LISTSTART(client.getNickName()));
        for (it = channels.begin(); it != channels.end(); ++it) {
            std::stringstream ss;
            ss << it->clients.size();
            client.ServerToClientPrefix(RPL_LIST(client.getNickName(), it->getchannelName(), ss.str(), it->getTopic()));
        }
        client.ServerToClientPrefix(RPL_LISTEND(client.getNickName()));
        ///////////////change this
        if (it == channels.end()) {
            client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(),trim(arguments[1])));
        }
    } else {
        //your code here (use '*' as wildcard)
    }
}