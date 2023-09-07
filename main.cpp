#include "Server.hpp"

int main(int ac, char* av[]){
    if (ac != 3) {
        std::cerr << "Usage: ./ircserv [port] [password]" << std::endl;
        return 0;
    }
    try {
        Server test(getHostIpAddress(), parsing(av[1]), trim(av[2]));
        test.run();
    }
    catch (std::exception& e){
        std::cout << e.what() << std::endl;
    }
    return 0;
}