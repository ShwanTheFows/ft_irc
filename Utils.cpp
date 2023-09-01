#include "Utils.hpp"

std::vector<std::string> splitString(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream inputStringStream(str);
    std::string token;
    while (inputStringStream >> token)
        tokens.push_back(token);
    return tokens;
}

std::string tolower(const std::string& input) {
    std::string result = input;
    for (size_t i = 0; i < result.length(); ++i)
        result[i] = std::tolower(result[i]);
    return result;
}

int parsing(const std::string& str) {
    int port;

    if (str.empty()) {
        std::cerr << "Please enter port number!" << std::endl;
        exit(1);
    }

    size_t i = 0;

    if (str[i] == '+' || str[i] == '-')
        ++i;

    for (; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            std::cerr << "The port should be a number!" << std::endl;
            exit(1);
        }
    }

    port = std::atoi(str.c_str());

    if (port < 1024 || port > 65535) {
        std::cerr << "Invalid port number!" << std::endl;
        exit(1);
    }

    return port;
}

std::string removeTrailingWhitespace(const std::string& str) {
    int endIndex = str.size() - 1;
    while (endIndex >= 0 && std::isspace(str[endIndex]))
        endIndex--;
    return str.substr(0, endIndex + 1);
}

std::string getHostIpAddress(void) {
    std::string ipAddress;
    FILE* pipe = popen("ifconfig | grep 'inet ' | grep -v 127.0.0.1 | awk '{print $2}' | head -n 1", "r");
    if (pipe != NULL) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != NULL)
            ipAddress = buffer;
        pclose(pipe);
    }
    return removeTrailingWhitespace(ipAddress);
}


bool isValidNickname(const std::string& nickname) {
    if (nickname.empty() || !isalpha(nickname[0]) || nickname.length() > 9) return false;

    for (size_t i = 1; i < nickname.length(); ++i) {
        char ch = nickname[i];
        if (!isalnum(ch) && ch != '_' && ch != '-') return false;
    }
    return true;
}

bool isValidUsername(const char* username) {
    // Username must be non-empty
    if (strlen(username) == 0)
        return false;

    // Check each character of the username
    for (size_t i = 0; i < strlen(username); ++i) {
        // Username can only contain alphanumeric characters, underscores, and dashes
        if (!isalnum(username[i]) && username[i] != '_' && username[i] != '-')
            return false;
    }

    // Username should not start or end with underscores or dashes
    if (username[0] == '_' || username[0] == '-' || username[strlen(username) - 1] == '_' || username[strlen(username) - 1] == '-')
        return false;

    return true;
}

