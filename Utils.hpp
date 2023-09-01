#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <cctype>

std::vector<std::string> splitString(const std::string& str);
std::string tolower(const std::string& input);
int parsing(const std::string& str);
std::string removeTrailingWhitespace(const std::string& str);
std::string getHostIpAddress(void);
bool isValidNickname(const std::string& nickname);
bool isValidUsername(const char* username);

#endif