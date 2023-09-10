#include "../Server.hpp"

void Server::topic(Client& client, std::vector<std::string>& arguments) {
    Channel* ch;
    if (arguments.size() >= 2)
        ch = getChannel(trim(arguments[1]));
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (!doesChannelExist(trim(arguments[1]))) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
    else if (!doesClientExistInChannel(*ch, client.getNickName())) client.ServerToClientPrefix(ERR_USERNOTINCHANNEL(client.getNickName(), "", trim(arguments[1])));
    else if (arguments.size() == 2 && ch->getTopic().empty()) client.ServerToClientPrefix(RPL_NOTOPIC(client.getNickName(), ch->getchannelName()));
    else if (arguments.size() == 2) {
        client.ServerToClientPrefix(RPL_TOPIC(client.getNickName(), ch->getchannelName(), ch->getTopic()));
    } else if (arguments.size() > 2) {
        if (tolower(ch->getchannelName()) == tolower(trim(arguments[1]))) {
            if (ch->hasTopic()) {
                if(ch->isOp(client.getNickName())){
                    if (trim(joinVectorFromIndex(arguments,2)).empty()) client.ServerToClientPrefix(RPL_NOTOPIC(client.getNickName(), ch->getchannelName()));
                    else {
                        if (trim(arguments[2])[0] == ':')
                            ch->setTopic(joinVectorFromIndex(arguments,2));
                        else
                            ch->setTopic(trim(arguments[2]));
                        sendToChannelMembers(ch, client, "TOPIC " + ch->getchannelName() + " " + ch->getTopic());    
                    }
                }
                else
                    client.ServerToClientPrefix(ERR_CHANOPRIVSNEEDED(client.getNickName(), ch->getchannelName()));
            }
            else {
                if (trim(joinVectorFromIndex(arguments,2)).empty()) client.ServerToClientPrefix(RPL_NOTOPIC(client.getNickName(), ch->getchannelName()));
                else {
                    if (trim(arguments[2])[0] == ':')
                        ch->setTopic(joinVectorFromIndex(arguments,2));
                    else
                        ch->setTopic(trim(arguments[2]));
                    sendToChannelMembers(ch, client, "TOPIC " + ch->getchannelName() + " " + ch->getTopic());    
                }
            }
            return ;
        }
    }
}