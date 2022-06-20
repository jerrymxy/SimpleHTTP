#ifndef _UTILS_H_
#define _UTILS_H_
#include <vector>
#include <map>
#include <ctime>
#include <string>
#include <fstream>

// Split every line into a vector
std::vector<std::string> splitString(std::string str);
// Generate a map that show the request fields
std::map<std::string, std::string> getRequestField(const std::vector<std::string>& split_str);
// Generate a map that splits the param from the request
std::map<std::string, std::string> getRequestParam(std::string source);
// Read from file
std::string readFile(std::string filepath);
// Get current time
std::string getDate();

#endif
