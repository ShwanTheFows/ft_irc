#include "../Server.hpp"

void Server::quit(Client& client, std::vector<std::string>& arguments) {
    std::string goodByeMessage;

    if (arguments.size() > 1) goodByeMessage = ":" + joinVectorFromIndex(arguments, 1);
    else goodByeMessage = client.getNickName();

    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
        if (doesClientExistInChannel(*it, client.getNickName())) {
            it->removeMember(client.getNickName());
            sendToChannelMembers(&*it, client, "QUIT :Quit: " + goodByeMessage);
        }
    }

    client.mySend("ERROR :Closing Link: " + client.getServerHostName() + " (Quit: " + goodByeMessage + ")");
    std::cout << client.getNickName() << " left the server." << std::endl;
    // removing the client from the client array
    close(client.getClientSocket());
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++) {
        if (client.getClientSocket() == it->first) {
            clients.erase(it);
            break;
        }
    }
}