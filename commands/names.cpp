#include "../Server.hpp"

void Server::names(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() > 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 2) {
        //your code
    } else if (arguments.size() == 1) {
        client.ServerToClientPrefix(RPL_ENDOFNAMES(client.getNickName(), "*"));
    }
}