#include "../Server.hpp"

void Server::quit(Client& client, std::vector<std::string>& arguments) {
    std::string goodByeMessage;

    if (arguments.size() > 1) goodByeMessage = joinVectorFromIndex(arguments, 1);
    else goodByeMessage = client.getNickName();

    for (std::vector<channel>::iterator it = channels.begin(); it != channels.end(); it++) {
        if (doesClientExistInChannel(*it, client.getNickName())) {
            sendToChannelMembers(&*it, client, "QUIT :Quit: " + goodByeMessage);
            it->removeMember(client.getNickName());
        }
    }

    client.mySend("ERROR :Closing Link: " + client.getServerHostName() + " (Quit: " + goodByeMessage + ")");
    // removing the client from the client array
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++) {
        if (client.getClientSocket() == it->first) {
            clients.erase(it);
            break;
        }
    }
    std::cout << client.getNickName() << " left the server." << std::endl;
    close(client.getClientSocket());
}