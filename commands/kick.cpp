#include "../Server.hpp"

void Server::kick(Client& client, std::vector<std::string>& arguments) {
    Channel* ch;
    if (arguments.size() >= 2)
        ch = getChannel(trim(arguments[1]));
    if (arguments.size() < 3) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (trim(arguments[1])[0] != '#') client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
    else if (!doesChannelExist(trim(arguments[1]))) client.ServerToClientPrefix(ERR_NOSUCHCHANNEL(client.getNickName(), trim(arguments[1])));
    else if (!ch->isOp(client.getNickName())) client.ServerToClientPrefix(ERR_CHANOPRIVSNEEDED(client.getNickName(), trim(arguments[1])));
    else if (!doesClientExistInChannel(*ch, trim(arguments[2]))) client.ServerToClientPrefix(ERR_USERNOTINCHANNEL(client.getNickName(), "", trim(arguments[1])));
    else {
        std::string reason;
        if (arguments.size() > 3) reason = joinVectorFromIndex(arguments, 3);
        else reason = ":" + client.getNickName();
        sendToChannelMembers(ch, client, "KICK " + ch->getchannelName() + " " + trim(arguments[2]) + " " + reason);
        ch->removeMember(trim(arguments[2]));
    }
}