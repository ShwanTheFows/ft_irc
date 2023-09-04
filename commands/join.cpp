#include "../Server.hpp"

// channel* Server::isChannel(std::string arg)
// {
//     std::vector<channel>::iterator it;
//     for (it = channels.begin(); it != channels.end(); ++it) {
//         if (it->getchannelName() == arg)
//             return &(*it);
//     }
    
//     // if (it == channels.end()) {
        
//     //     return &channels.back();
//     // }
    
//     return nullptr;
// }

void Server::join(Client& client, std::vector<std::string>& arguments) {
    if (arguments.size() < 2) client.ServerToClientPrefix(ERR_NEEDMOREPARAMS(client.getNickName()));
    else if (!isValidChannelName(trim(arguments[1]))) client.ServerToClientPrefix(ERR_BADCHANNAME(client.getNickName()));
    else {
        std::vector<channel>::iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            if (it->getchannelName() == trim(arguments[1])) {
                if(it->getPrivate() == false){
                    if (it->sethaveKey()) {
                        if(it->getKey() == trim(arguments[3]))
                            it->addMember(client);
                        else
                            std::cout << "wrong password!" << std::endl;
                    }
                    else
                        it->addMember(client);
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
                client.ServerToClientPrefix(USR_JOIN(client.getNickName(),channels[channels.size()-1].getchannelName()));
        }
    }
}