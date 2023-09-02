#include "../Server.hpp"

std::string getIdle(std::time_t time)
{
    std::stringstream ss;
    ss << std::difftime(std::time(NULL), time);
    return ss.str();
}

void Server::whois(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 2 || arguments.size() > 3) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (arguments.size() == 3 && trim(arguments[2]) != client.getServerHostName()) client.ServerToClientPrefix(ERR_NOSUCHSERVER(client.getNickName(), trim(arguments[2])));
    else if (arguments.size() == 1) client.ServerToClientPrefix(ERR_NONICKNAMEGIVEN(client.getNickName()));
    else {
        for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.getNickName() == trim(arguments[1])) {
                Client clientCopy = it->second;
                //check if user has joined any channels
                client.ServerToClientPrefix(RPL_WHOISUSER(client.getNickName(), trim(arguments[1]), it->second.getNickName(), it->second.getHostName(), it->second.getRealName()));
                client.ServerToClientPrefix(RPL_WHOISSERVER(client.getNickName(), it->second.getNickName(), client.getHostName(), "ft_irc"));
                client.ServerToClientPrefix(RPL_WHOISIDLE(client.getNickName(), it->second.getNickName(), getIdle(client.timeJoined), client.getJoiningTime()));
                return ;
            }
        }
        client.ServerToClientPrefix(ERR_NOSUCHNICK(client.getNickName()));
    }
}