#include "../Server.hpp"

void Server::names(Client& client, std::vector<std::string>& arguments) {
    channel* ch;
    if (arguments.size() == 2)
        ch = getChannel(trim(arguments[1]));
    if (arguments.size() > 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 2 && !doesChannelExist(trim(arguments[1]))) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
    else if (arguments.size() == 2 && !doesClientExistInChannel(*ch, client.getNickName())) client.ServerToClientPrefix(ERR_USERNOTINCHANNEL(client.getNickName(), "", trim(arguments[1])));
    else if (arguments.size() == 2) {
        client.ServerToClientPrefix(RPL_NAMREPLY(client.getNickName(), ch->getchannelName(), ch->getClientNames()));
        client.ServerToClientPrefix(RPL_ENDOFNAMES(client.getNickName(), ch->getchannelName()));
    } else if (arguments.size() == 1) {
        client.ServerToClientPrefix(RPL_NAMREPLY(client.getNickName(), "*", "*"));
        client.ServerToClientPrefix(RPL_ENDOFNAMES(client.getNickName(), "*"));
    }
}