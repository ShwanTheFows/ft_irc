#include "../Server.hpp"

void Server::notice(Client& client, std::vector<std::string>& arguments) {
    Channel* ch;
    if (arguments.size() >= 2)
        ch = getChannel(trim(arguments[1]));
    if (arguments.size() < 3) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() >= 3 && trim(arguments[1])[0] == ':') client.ServerToClientPrefix(ERR_NORECIPIENT(client.getNickName(), joinVectorFromIndex(arguments, 0)));
    else if (arguments.size() >= 3 && trim(arguments[2])[0] != ':') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (trim(arguments[1])[0] != '#') {
        for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.getNickName() == trim(arguments[1])) {
                Client clientCopy = it->second;
                sendMessageToClient(client,  "NOTICE " + it->second.getNickName() + " :" + joinVectorFromIndex(arguments, 2), clientCopy.getClientSocket());
                return ;
            }
        }
        client.ServerToClientPrefix(ERR_NOSUCHNICK(trim(arguments[1])));
    } else if (trim(arguments[1])[0] == '#') {
        if (!doesChannelExist(trim(arguments[1]))) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
        else if (!doesClientExistInChannel(*ch, client.getNickName())) client.ServerToClientPrefix(ERR_USERNOTINCHANNEL(client.getNickName(), "", trim(arguments[1])));
        else sendToChannelMembers(ch, client, "NOTICE " + client.getNickName() + " :" + joinVectorFromIndex(arguments, 2));
    }
}