#include "../Server.hpp"

void Server::join(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (!isValidChannelName(trim(arguments[1]))) client.ServerToClientPrefix(ERR_BADCHANNAME(client.getNickName()));
    else {
        //your code
    }
}