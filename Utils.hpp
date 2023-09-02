#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <cctype>
#include <algorithm>

std::vector<std::string> splitString(const std::string& str);
std::string joinVectorFromIndex(const std::vector<std::string>& input, size_t startIndex);
std::string tolower(const std::string& input);
int parsing(const std::string& str);
std::string trim(const std::string& str);
std::string removeTrailingWhitespace(const std::string& str);
void removeTrailingNewline(char* str);
std::string getHostIpAddress(void);
bool isValidNickname(const std::string& nickname);
bool isValidUsername(const std::string& username);
bool isValidChannelName(const std::string& channelName);

#endif
