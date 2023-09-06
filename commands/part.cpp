#include "../Server.hpp"

void Server::part(Client& client, std::vector<std::string>& arguments) {
    Channel* ch;
    if (arguments.size() >= 2)
        ch = getChannel(trim(arguments[1]));
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (trim(arguments[1])[0] != '#') client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
    else if (!doesChannelExist(trim(arguments[1]))) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
    else if (!doesClientExistInChannel(*ch, client.getNickName())) client.ServerToClientPrefix(ERR_USERNOTINCHANNEL(client.getNickName(), "", trim(arguments[1])));
    else if (arguments.size() >= 2) {
        std::string goodByeMessage;
        if (arguments.size() > 1) goodByeMessage =  ":" + joinVectorFromIndex(arguments, 1);
        else goodByeMessage =  client.getNickName();
        sendToChannelMembers(ch, client, "PART " + goodByeMessage);
        ch->removeMember(client.getNickName());
    }
}