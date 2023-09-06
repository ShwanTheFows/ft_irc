#include "../Server.hpp"

void Server::topic(Client& client, std::vector<std::string>& arguments) {
    channel* ch;
    if (arguments.size() >= 2)
        ch = getChannel(trim(arguments[1]));
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() > 2 && trim(arguments[2])[0] != ':') client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (!doesChannelExist(trim(arguments[1]))) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
    else if (!doesClientExistInChannel(*ch, client.getNickName())) client.ServerToClientPrefix(ERR_USERNOTINCHANNEL(client.getNickName(), "", trim(arguments[1])));
    else if (arguments.size() == 2) {
        client.ServerToClientPrefix(RPL_TOPIC(client.getNickName(), ch->getchannelName(), ch->getTopic()));
    } else if (arguments.size() > 2) {
        if (ch->getchannelName() == arguments[1]) {
            if(ch->hasTopic() )
                if(ch->isOp(client.getNickName())){
                    ch->setTopic(joinVectorFromIndex(arguments,2));
                    client.ServerToClientPrefix(RPL_TOPIC(client.getNickName(), ch->getchannelName(), ch->getTopic()));
                }
                else
                    client.ServerToClientPrefix(ERR_CANNOTSENDTOCHAN(client.getNickName()));
            else {
                ch->setTopic(joinVectorFromIndex(arguments,2));
                client.ServerToClientPrefix(RPL_TOPIC(client.getNickName(), ch->getchannelName(), ch->getTopic()));
            }
            return ;
        }
    }
}