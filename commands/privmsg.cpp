#include "../Server.hpp"

void Server::privmsg(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() >= 2 && trim(arguments[1])[0] == ':') client.ServerToClientPrefix(ERR_NORECIPIENT(client.getNickName(), joinVectorFromIndex(arguments, 0)));
    else if (arguments.size() > 2 && trim(arguments[2])[0] != ':') client.ServerToClientPrefix(ERR_TOOMANYTARGETS(trim(arguments[1])));
    else if (trim(arguments[1])[0] != '#') {
        for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.getNickName() == trim(arguments[1])) {
                Client clientCopy = it->second;
                sendMessageToClient(clientCopy, joinVectorFromIndex(arguments, 2));
                return ;
            }
        }
        client.ServerToClientPrefix(ERR_NOSUCHNICK(trim(arguments[1])));
    } else if (trim(arguments[1])[0] == '#') {
        //your code here
    }
}