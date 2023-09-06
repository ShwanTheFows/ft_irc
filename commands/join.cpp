#include "../Server.hpp"

void    printJoinInfo(Client &client, channel& channel, std::string clientNames) {
    if (!channel.getTopic().empty())
        client.ServerToClientPrefix(RPL_TOPIC(client.getNickName(), channel.getchannelName(), channel.getTopic()));
    client.ServerToClientPrefix(RPL_NAMREPLY(client.getNickName(), channel.getchannelName(), clientNames));
    client.ServerToClientPrefix(RPL_ENDOFNAMES(client.getNickName(), channel.getchannelName()));
}

void Server::join(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 2 || arguments.size() > 3) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (!isValidChannelName(trim(arguments[1]))) client.ServerToClientPrefix(ERR_BADCHANNAME(client.getNickName()));
    else {
        std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (it->getchannelName() == trim(arguments[1])) {
                if(it->getPrivate() == false){
                    if (it->sethaveKey() && arguments.size() == 3) {
                        if(arguments.size() == 3 && it->getKey() == trim(arguments[2])) {
                            if (!it->addMember(client)) return ;
                            sendToChannelMembers(&(*it), client, "JOIN :" + it->getchannelName());
                            printJoinInfo(client, *it, it->getClientNames());
                        }
                        else
                            client.ServerToClientPrefix(ERR_PASSWDMISMATCH(client.getNickName()));
                    }
                    else if (it->sethaveKey() && arguments.size() == 2) client.ServerToClientPrefix(ERR_BADCHANNELKEY(client.getNickName(), it->getchannelName()));
                    else {
                        if (!it->addMember(client)) return ;
                        sendToChannelMembers(&(*it), client, "JOIN :" + it->getchannelName());
                        printJoinInfo(client, *it, it->getClientNames());
                    }
                }
                else if (it->getPrivate() == true) {
                    channel* ch = getChannel(trim(arguments[1]));
                    if (ch->isInInviteList(client.getNickName())) {
                        if (it->sethaveKey() && arguments.size() == 3) {
                            if(arguments.size() == 3 && it->getKey() == trim(arguments[2])) {
                                if (!it->addMember(client)) return ;
                                sendToChannelMembers(&(*it), client, "JOIN :" + it->getchannelName());
                                printJoinInfo(client, *it, it->getClientNames());
                            }
                            else
                                client.ServerToClientPrefix(ERR_PASSWDMISMATCH(client.getNickName()));
                        }
                        else if (it->sethaveKey() && arguments.size() == 2) client.ServerToClientPrefix(ERR_BADCHANNELKEY(client.getNickName(), it->getchannelName()));
                        else {
                            if (!it->addMember(client)) return ;
                            sendToChannelMembers(&(*it), client, "JOIN :" + it->getchannelName());
                            printJoinInfo(client, *it, it->getClientNames());
                        }
                    } else client.ServerToClientPrefix(ERR_INVITEONLYCHAN(client.getNickName(), it->getchannelName()));
                }
                else
                   client.ServerToClientPrefix(ERR_INVITEONLYCHAN(client.getNickName(), it->getchannelName()));
                break;
            }
        }

        if (it == channels.end()) {
                if (arguments.size() ==  3)
                    channels.push_back(channel(trim(arguments[1]),client,trim(arguments[2])));
                else if (arguments.size() ==  2)
                    channels.push_back(channel(trim(arguments[1]),client));
                sendToChannelMembers(&channels[channels.size()-1], client, "JOIN :" + channels[channels.size()-1].getchannelName());
                printJoinInfo(client, channels[channels.size()-1], channels[channels.size()-1].getClientNames());
        }
    }
}