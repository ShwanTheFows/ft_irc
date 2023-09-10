#include "../Server.hpp"

void Server::part(Client& client, std::vector<std::string>& arguments) {
    Channel* ch;
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (trim(arguments[1])[0] != '#') client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
    else if (arguments.size() >= 2) {
        std::string goodByeMessage = "";
        if (arguments.size() > 2) {
            if (trim(arguments[2])[0] == ':')
                goodByeMessage = ":" + joinVectorFromIndex(arguments, 2);
            else
                goodByeMessage = ":" + trim(arguments[2]);
        }
        std::vector<std::string> chnls = splitStringByComma(trim(arguments[1]));
        for (std::vector<std::string>::iterator element = chnls.begin(); element != chnls.end(); element++) {
                ch = getChannel(*element);
                if (!doesChannelExist(*element)) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), *element));
                else if (!doesClientExistInChannel(*ch, client.getNickName())) client.ServerToClientPrefix(ERR_USERNOTINCHANNEL(client.getNickName(), "", *element));
                else {
                    sendToChannelMembers(ch, client, "PART " + ch->getchannelName() + goodByeMessage);
                    ch->removeMember(client.getNickName());
                    if (ch->clients.size() == 0) {
                        removeChannel(*element);
                    }
                }
        }
    }
}