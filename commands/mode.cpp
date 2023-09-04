#include "../Server.hpp"

void Server::mode(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 3) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() > 3 && trim(arguments[2])[0] == '-') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 3 && trim(arguments[2])[0] == '+') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 3 && trim(arguments[2])[0] == '-') {
        // your code here
    }
}