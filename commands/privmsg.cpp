#include "../Server.hpp"

void Server::privmsg(Client& client, std::vector<std::string>& arguments) {
    Channel* ch;
    if (arguments.size() < 3) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() >= 2 && trim(arguments[1])[0] == ':') client.ServerToClientPrefix(ERR_NORECIPIENT(client.getNickName(), joinVectorFromIndex(arguments, 0)));
    else if ((arguments.size() > 2 && trim(arguments[2])[0] == ':' && trim(joinVectorFromIndex(arguments, 2)).length() <= 1) || (trim(joinVectorFromIndex(arguments, 2)).length() == 0)) client.ServerToClientPrefix(ERR_NOTEXTTOSEND(client.getNickName()));
    else {
        std::vector<std::string> receivers = splitStringByComma(trim(arguments[1]));
        for (std::vector<std::string>::iterator element = receivers.begin(); element != receivers.end(); element++) {
            if ((*element)[0] != '#') {
                if (!clientExists(*element)) {
                    client.ServerToClientPrefix(ERR_NOSUCHNICK(trim(arguments[1])));
                    continue;
                }
                for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
                    if (tolower(it->second.getNickName()) == tolower(*element)) {
                        if (trim(arguments[2])[0] == ':')
                            sendMessageToClient(client, "PRIVMSG " + it->second.getNickName() + " :" + joinVectorFromIndex(arguments, 2), it->second.getClientSocket());
                        else
                            sendMessageToClient(client, "PRIVMSG " + it->second.getNickName() + " :" + trim(arguments[2]), it->second.getClientSocket());
                        break ;
                    }
                }
            } else if ((*element)[0] == '#') {
                ch = getChannel(*element);
                if (!doesChannelExist(*element)) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), *element));
                else if (!doesClientExistInChannel(*ch, client.getNickName())) client.ServerToClientPrefix(ERR_USERNOTINCHANNEL(client.getNickName(), "", *element));
                else {
                    if (trim(arguments[2])[0] == ':')
                        sendToChannelMembersExceptClient(ch, client, "PRIVMSG " + ch->getchannelName() + " :" + joinVectorFromIndex(arguments, 2));
                    else
                        sendToChannelMembersExceptClient(ch, client, "PRIVMSG " + ch->getchannelName() + " :" + trim(arguments[2]));
                }
            }
        }
    }
}
