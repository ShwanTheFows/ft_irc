#include "Utils.hpp"

std::string passLimit(std::string input) {
    if (input.length() > 23)
        return input.substr(0, 23);
    return input;
}

void fillVectorFromEnd(std::vector<std::string>& vector, int index) {
    int currentLength = vector.size();
    if (index <= currentLength) {
        return;
    }

    int numEmptyStrings = index - currentLength;
    for (int i = 0; i < numEmptyStrings; ++i) {
        vector.push_back("");
    }
}

std::vector<std::string> splitStringByComma(const std::string& input) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, ',')) {
        result.push_back(item);
    }

    return result;
}

std::vector<std::string> splitStringByNewline(const std::string& input) {
    std::vector<std::string> result;
    std::istringstream iss(input);
    std::string line;

    while (std::getline(iss, line)) {
        result.push_back(line);
    }

    return result;
}

std::vector<std::string> splitString(const std::string& str) {
    size_t i  = 0;
    std::string element = "";
    std::vector<std::string> result;
    while (str[i]) {
        element += str[i];
        if (std::isspace(str[i]) || str[i + 1] == '\0') {
            for (size_t j = 0; j < element.length(); j++) {
                if (!std::isspace(element[j])) {
                    result.push_back(removeTrailingWhitespace(element));
                    element = "";
                }
            }
        }
        i++;
    }
    return result;
}

std::string joinVectorFromIndex(const std::vector<std::string>& input, size_t startIndex) {
    std::string result;

    if (startIndex < 0 || startIndex >= input.size())
        return result;

    for (size_t i = startIndex; i < input.size(); ++i) {
        result += input[i];
        if (i != input.size() - 1)
            result += " ";
    }
    int i = 0;
    while (input[startIndex][i] && std::isspace(input[startIndex][i]))
        i++;
    if (input[startIndex][i] == ':') return result.substr(i + 1); 
    return result;
}

std::string joinVectorFromIndex2(const std::vector<std::string>& input, size_t startIndex) {
    std::string result;

    if (startIndex < 0 || startIndex >= input.size())
        return result;  // Return an empty string if the start index is out of bounds

    for (size_t i = startIndex; i < input.size(); ++i) {
        result += trim(input[i]);
    }
    return result;
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
    std::size_t start = 0;
    while (start < str.length() && std::isspace(str[start]))
        ++start;

    std::size_t end = str.length();
    while (end > start && std::isspace(str[end - 1]))
        --end;

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

void removeTrailingNewline(std::string& str) {
    if (str.empty())
        return;

    size_t length = str.length();
    while (length > 0 && (str[length - 1] == '\r' || str[length - 1] == '\n')) {
        str.erase(length - 1);
        length = str.length();
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
    if (username.empty())
        return false;

    for (size_t i = 0; i < username.length(); ++i) {
        if (!isalnum(username[i]) && username[i] != '_' && username[i] != '-')
            return false;
    }

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
