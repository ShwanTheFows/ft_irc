#include "../Server.hpp"

void Server::names(Client& client, std::vector<std::string>& arguments) {
    Channel* ch;
    if (arguments.size() == 2)
        ch = getChannel(trim(arguments[1]));
    if (arguments.size() > 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 2) {
        std::vector<std::string> chnls = splitStringByComma(trim(arguments[1]));
        for (std::vector<std::string>::iterator element = chnls.begin(); element != chnls.end(); element++) {
                ch = getChannel(*element);
                if (!doesChannelExist(*element)) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), *element));
                else if (!doesClientExistInChannel(*ch, client.getNickName())) client.ServerToClientPrefix(ERR_USERNOTINCHANNEL(client.getNickName(), "", *element));
                else {
                    client.ServerToClientPrefix(RPL_NAMREPLY(client.getNickName(), ch->getchannelName(), ch->getClientNames()));
                    client.ServerToClientPrefix(RPL_ENDOFNAMES(client.getNickName(), ch->getchannelName()));
                }
        }
    } else if (arguments.size() == 1) {
        client.ServerToClientPrefix(RPL_NAMREPLY(client.getNickName(), "*", "*"));
        client.ServerToClientPrefix(RPL_ENDOFNAMES(client.getNickName(), "*"));
    }
}