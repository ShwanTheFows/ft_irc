#include "Utils.hpp"

std::vector<std::string> splitString(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream inputStringStream(str);
    std::string token;
    while (inputStringStream >> token) {
        std::size_t spacesCount = token.find_first_not_of(" \t");
        std::string tokenWithSpaces = std::string(spacesCount, ' ') + token;
        tokens.push_back(tokenWithSpaces);
    }
    return tokens;
}

std::string joinVectorFromIndex(const std::vector<std::string>& input, size_t startIndex) {
    std::string result;

    if (startIndex < 0 || startIndex >= input.size())
        return result;  // Return an empty string if the start index is out of bounds

    for (size_t i = startIndex; i < input.size(); ++i) {
        result += input[i];
        if (i != input.size() - 1)
            result += " ";  // Add a space if it's not the last element
    }
    if (result[0] == ':') return result.substr(1);
    else return result;
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

std::string trim(const std::string& str) {
    // Find the first non-space character from the beginning
    std::size_t start = 0;
    while (start < str.length() && std::isspace(str[start]))
        ++start;

    // Find the first non-space character from the end
    std::size_t end = str.length();
    while (end > start && std::isspace(str[end - 1]))
        --end;

    // Return the trimmed substring
    return str.substr(start, end - start);
}

std::string removeTrailingWhitespace(const std::string& str) {
    int endIndex = str.size() - 1;
    while (endIndex >= 0 && std::isspace(str[endIndex]))
        endIndex--;
    return str.substr(0, endIndex + 1);
}

void removeTrailingNewline(char* str) {
    if (str == NULL)
        return;
        
    size_t length = strlen(str);
    while (length > 0 && (str[length - 1] == '\r' || str[length - 1] == '\n')) {
        str[length - 1] = '\0';
        length--;
    }
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

bool isValidUsername(const std::string& username) {
    // Username must be non-empty
    if (username.empty())
        return false;

    // Check each character of the username
    for (size_t i = 0; i < username.length(); ++i) {
        // Username can only contain alphanumeric characters, underscores, and dashes
        if (!isalnum(username[i]) && username[i] != '_' && username[i] != '-')
            return false;
    }

    // Username should not start or end with underscores or dashes
    if (username[0] == '_' || username[0] == '-' || username[username.length() - 1] == '_' || username[username.length() - 1] == '-')
        return false;

    return true;
}

bool isValidChannelName(const std::string& channelName) {
    if (channelName.empty() || channelName[0] != '#' || channelName.length() <= 1)
        return false;

    for (size_t i = 1; i < channelName.length(); ++i) {
        if (!isalnum(channelName[i]) && channelName[i] != '_')
            return false;
    }

    return true;
}
