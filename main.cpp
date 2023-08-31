#include "Server.hpp"

int main(int ac, char* av[]){
    if (ac != 3) {
        std::cerr << "Usage: ./ircserv [port] [passoword]";
        return 0;
    }
    try {
        Server test("127.0.0.1", parsing(av[1]), av[2]);
        test.setUpSocket();
        test.bind();
        test.listen();
        test.disconnect();
    }
    catch (std::exception& e){
        std::cout << e.what() << std::endl;
    }
    return 0;
}