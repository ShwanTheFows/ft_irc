#include "../Server.hpp"

void Server::invite(Client& client, std::vector<std::string>& arguments) {
    Channel* ch;
    if (arguments.size() == 3)
        ch = getChannel(trim(arguments[2]));
    if (arguments.size() != 3) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (!doesChannelExist(trim(arguments[2]))) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[2])));
    else if (!doesClientExistInChannel(*ch, client.getNickName())) client.ServerToClientPrefix(ERR_USERNOTINCHANNEL(client.getNickName(), "", trim(arguments[2])));
    else if (arguments.size() == 3) {
        if (!ch->isOp(client.getNickName())) client.ServerToClientPrefix(ERR_CHANOPRIVSNEEDED(client.getNickName(), ch->getchannelName()));
        else if (!clientExists(trim(arguments[1]))) client.ServerToClientPrefix(ERR_NOSUCHNICK(client.getNickName()));
        Client* clt = getClient(trim(arguments[1]));
        ch->addToInviteList(clt->getNickName());
        client.ServerToClientPrefix(RPL_INVITING(client.getNickName(), clt->getNickName(), ch->getchannelName()));
        client.ServerToClientPrefix("NOTICE @" + ch->getchannelName() + " :" + client.getNickName() + " invited " + clt->getNickName() + " into Channel " + ch->getchannelName());
        sendMessageToClient(client, "INVITE " + clt->getNickName() + " :" + ch->getchannelName(), clt->getClientSocket());
    }
}