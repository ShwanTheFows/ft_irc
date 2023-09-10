#include "../Server.hpp"

std::string getIdle(std::time_t time)
{
    std::stringstream ss;
    ss << std::difftime(std::time(NULL), time);
    return ss.str();
}

void Server::whois(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() == 1) client.ServerToClientPrefix(ERR_NONICKNAMEGIVEN(client.getNickName()));
    else if (arguments.size() > 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() > 1) {
        int first = 1;
        std::string firstClient;
        std::vector<std::string> clts = splitStringByComma(joinVectorFromIndex2(arguments, 1));
        for (size_t i = 0; i < clts.size(); i++) {
            if (!clientExists(clts[i])) {
                client.ServerToClientPrefix(ERR_NOSUCHNICK(clts[i]));
                continue;
            }
            if (first == 1) {
                firstClient = clts[i];
                first = 0;
            }
            std::string chnls;
            Client* clt = getClient(clts[i]);
            for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
                if(doesClientExistInChannel(*it, clts[i])) {
                    if (it->isOp(clts[i])) chnls += "@" + it->getchannelName() + " ";
                    else chnls += it->getchannelName() + " ";
                }
            }
            if (!chnls.empty())
                client.ServerToClientPrefix(RPL_WHOISCHANNELS(client.getNickName(), clts[i], chnls));
            client.ServerToClientPrefix(RPL_WHOISUSER(client.getNickName(), clts[i], "", client.getHostName(), clt->getRealName()));
            client.ServerToClientPrefix(RPL_WHOISSERVER(client.getNickName(), clts[i], client.getServerHostName(), "ft_irc"));
            client.ServerToClientPrefix(RPL_WHOISIDLE(client.getNickName(), clts[i], getIdle(clt->timeJoined), clt->getJoiningTime()));
            if (i == clts.size() - 1) client.ServerToClientPrefix(RPL_ENDOFWHOIS(client.getNickName(), firstClient));
        }
    }
}