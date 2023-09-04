#include "../Server.hpp"

void Server::part(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (trim(arguments[1])[0] == '#') client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
    else if (arguments.size() >= 2) {
        std::string goodByeMessage;
        if (arguments.size() > 1) goodByeMessage = joinVectorFromIndex(arguments, 1);
        else goodByeMessage = client.getNickName();
        //your code
    }
}