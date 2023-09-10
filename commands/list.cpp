#include "../Server.hpp"

void Server::list(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() > 3) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 1) {
        std::vector<Channel>::iterator it;
        client.ServerToClientPrefix(RPL_LISTSTART(client.getNickName()));
        for (it = channels.begin(); it != channels.end(); ++it)
                client.ServerToClientPrefix(RPL_LIST(client.getNickName(), it->getchannelName(), it->getClientsSize(), it->getTopic()));
        client.ServerToClientPrefix(RPL_LISTEND(client.getNickName()));
    } else if (arguments.size() == 3 && trim(arguments[3]) != this->ip) client.ServerToClientPrefix(ERR_NOSUCHSERVER(client.getNickName(), trim(arguments[2])));
    else {
        client.ServerToClientPrefix(RPL_LISTSTART(client.getNickName()));
        std::vector<std::string> chnls = splitStringByComma(joinVectorFromIndex2(arguments, 1));
        for (size_t i = 0; i < chnls.size(); i++) {
            if (!doesChannelExist(chnls[i])) continue ;
            Channel* ch = getChannel(chnls[i]);
            client.ServerToClientPrefix(RPL_LIST(client.getNickName(), ch->getchannelName(), ch->getClientsSize(), ch->getTopic()));
        }
        client.ServerToClientPrefix(RPL_LISTEND(client.getNickName()));
    }
}