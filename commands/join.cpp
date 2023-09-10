#include "../Server.hpp"

void    printJoinInfo(Client &client, Channel& Channel, std::string clientNames) {
    if (!Channel.getTopic().empty())
        client.ServerToClientPrefix(RPL_TOPIC(client.getNickName(), Channel.getchannelName(), Channel.getTopic()));
    client.ServerToClientPrefix(RPL_NAMREPLY(client.getNickName(), Channel.getchannelName(), clientNames));
    client.ServerToClientPrefix(RPL_ENDOFNAMES(client.getNickName(), Channel.getchannelName()));
}

void Server::join(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 2 || arguments.size() > 3) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else {
        std::vector<std::string> receivers = splitStringByComma(trim(arguments[1]));
        std::vector<std::string> keys;
        if (arguments.size() >= 3) keys = splitStringByComma(trim(arguments[2]));
        if (receivers.size() > keys.size()) fillVectorFromEnd(keys, receivers.size());
        for (size_t i = 0; i < receivers.size(); i++) {
            Channel* ch = getChannel(receivers[i]);
            if (ch != NULL) {
                if(ch->getPrivate() == false){
                    if (ch->sethaveKey() && arguments.size() == 3) {
                        if(arguments.size() == 3 && ch->getKey() == keys[i]) {
                            if (!ch->addMember(client)) return ;
                            sendToChannelMembers(&(*ch), client, "JOIN :" + ch->getchannelName());
                            printJoinInfo(client, *ch, ch->getClientNames());
                            continue;
                        }
                        else {
                            client.ServerToClientPrefix(ERR_PASSWDMISMATCH(client.getNickName()));
                            continue;
                        }
                    }
                    else if (ch->sethaveKey() && arguments.size() == 2) {
                        client.ServerToClientPrefix(ERR_BADCHANNELKEY(client.getNickName(), ch->getchannelName()));
                        continue;
                    }
                    else {
                        if (!ch->addMember(client)) return ;
                        sendToChannelMembers(&(*ch), client, "JOIN :" + ch->getchannelName());
                        printJoinInfo(client, *ch, ch->getClientNames());
                        continue;
                    }
                }
                else if (ch->getPrivate() == true) {
                    if (ch->isInInviteList(client.getNickName())) {
                        if (!ch->addMember(client)) continue;
                        sendToChannelMembers(&(*ch), client, "JOIN :" + ch->getchannelName());
                        printJoinInfo(client, *ch, ch->getClientNames());
                        continue;
                    } else {
                        client.ServerToClientPrefix(ERR_INVITEONLYCHAN(client.getNickName(), ch->getchannelName()));
                        continue;
                    }
                }
                else {
                   client.ServerToClientPrefix(ERR_INVITEONLYCHAN(client.getNickName(), ch->getchannelName()));
                   continue;
                }
            }
            else {
                if (!isValidChannelName(receivers[i])) {
                    client.ServerToClientPrefix(ERR_BADCHANNAME(client.getNickName()));
                    continue;    
                }
                else if (!keys[i].empty())
                    channels.push_back(Channel(receivers[i],client,keys[i]));
                else
                    channels.push_back(Channel(receivers[i],client));
            }
            sendToChannelMembers(&channels[channels.size()-1], client, "JOIN :" + channels[channels.size()-1].getchannelName());
            printJoinInfo(client, channels[channels.size()-1], channels[channels.size()-1].getClientNames());
        }
    }
}