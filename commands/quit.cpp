#include "../Server.hpp"

void Server::quit(Client& client, std::vector<std::string>& arguments) {
    std::string goodByeMessage;

    if (arguments.size() > 1) goodByeMessage = joinVectorFromIndex(arguments, 1);
    else goodByeMessage = client.getNickName();

    // std::vector<Channels>::iterator it_channels = _Channels.begin();
    // while (it_channels != _Channels.end())
    // {
    //     if (checkUserIsInChannel(cl->second, &*it_channels))
    //     {
    //         send_msg_to_clients_who_in_channel(&*it_channels, cl->second, "QUIT :Quit: " + msg_quit);
    //         it_channels->removeMember(cl->second);
    //     }
    //     it_channels++;
    // } adir chi haja bhal haka

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