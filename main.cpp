#include "Server.hpp"

std::string removeTrailingWhitespace(const std::string& str) {
    int endIndex = str.size() - 1;
    while (endIndex >= 0 && std::isspace(str[endIndex])) {
        endIndex--;
    }
    return str.substr(0, endIndex + 1);
}

std::string getHostIpAddress() {
    std::string ipAddress;
    FILE* pipe = popen("ifconfig | grep 'inet ' | grep -v 127.0.0.1 | awk '{print $2}' | head -n 1", "r");
    if (pipe != NULL) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            ipAddress = buffer;
        }
        pclose(pipe);
    }

    return removeTrailingWhitespace(ipAddress);
}

int main(int ac, char* av[]){
    if (ac != 3) {
        std::cerr << "Usage: ./ircserv [port] [password]";
        return 0;
    }
    try {
        Server test(getHostIpAddress(), parsing(av[1]), av[2]);
        test.run();
    }
    catch (std::exception& e){
        std::cout << e.what() << std::endl;
    }
    return 0;
}