#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <cctype>
#include <algorithm>

void fillVectorFromEnd(std::vector<std::string>& vec, int endIndex);
std::vector<std::string> splitStringByComma(const std::string& input);
std::string passLimit(std::string input);
std::vector<std::string> splitStringByNewline(const std::string& input);
std::vector<std::string> splitString(const std::string& str);
std::string joinVectorFromIndex(const std::vector<std::string>& input, size_t startIndex);
std::string joinVectorFromIndex2(const std::vector<std::string>& input, size_t startIndex);
std::string tolower(const std::string& input);
int parsing(const std::string& str);
std::string trim(const std::string& str);
std::string removeTrailingWhitespace(const std::string& str);
void removeTrailingNewline(char* str);
void removeTrailingNewline(std::string& str);
std::string getHostIpAddress(void);
bool isValidNickname(const std::string& nickname);
bool isValidUsername(const std::string& username);
bool isValidChannelName(const std::string& channelName);

#endif
