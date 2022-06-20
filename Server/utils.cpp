#include "utils.hpp"

std::vector<std::string> splitString(std::string str) {
    size_t begin = 0, end = 0;
    std::vector<std::string> result;
    while (end < str.size() - 2) {
        end = str.find("\r\n", begin); // Location of CRLF
        std::string temp = str.substr(begin, end);
        if (temp != "\r\n")
            result.push_back(temp);
        begin = end + 2;
    }
    return result;
}

std::map<std::string, std::string> getRequestField(const std::vector<std::string>& split_str) {
    std::map<std::string, std::string> request_field;
    size_t blank_location = split_str[0].find_first_of(" ");
    std::string request_type = split_str[0].substr(0, blank_location);
    size_t end = split_str[0].find_last_of(' ');
    std::string url = split_str[0].substr(blank_location + 1, end - blank_location - 1); // URL between two blanks
    std::string http_version = split_str[0].substr(end + 1); // HTTP version in the last

    request_field["request_type"] = request_type;
    request_field["url"] = url;
    request_field["http_version"] = http_version;

    if (request_type == "GET") {
        // GET request has no body
        for (size_t i = 1; i < split_str.size(); i++) {
            size_t location = split_str[i].find(':');
            request_field[split_str[i].substr(0, location)] = split_str[i].substr(location + 2); // CRLF size = 2
        }
    } else if (request_type == "POST") {
        // POST request has body
        for (size_t i = 1; i < split_str.size() - 1; i++) {
            size_t location = split_str[i].find(':');
            request_field[split_str[i].substr(0, location)] = split_str[i].substr(location + 2);
        }
        request_field["Body"] = split_str[split_str.size() - 1]; // Split body data/param
    }

    
    return request_field;
}

std::map<std::string, std::string> getRequestParam(std::string source) {
    std::map<std::string, std::string> result;
    size_t and_location = source.find('&');

    // Split each param by &
    while (and_location != source.npos) {
        size_t eq_location = source.find('=');
        std::string param = source.substr(0, eq_location);
        result[param] = source.substr(eq_location + 1, and_location - eq_location - 1);
        source = source.substr(and_location + 1);
        and_location = source.find('&');
    }
    // Process last param
    size_t eq_location = source.find('=');
    if (eq_location == source.npos) {
        return result;
    }
    std::string param = source.substr(0, eq_location);
    result[param] = source.substr(eq_location + 1);
    return result;
}

std::string readFile(std::string filepath) {
    // Remove / mark to get files in current folder
    if (filepath[0] = '/') {
        filepath = filepath.substr(1);
    }
    std::string result;
    // Read file
    std::ifstream ifile;
    ifile.open(filepath);
    if (!ifile) {
        return "404";
    }
    char buf[1024];
    // Make sure characters per line < 1024
    while (ifile.getline(buf, 1023)) {
        result += std::string(buf);
    }
    ifile.close();
    return result;
}

std::string getDate() {
    char time_str_ch[50] = { 0 };
    time_t seconds;
    time(&seconds);
    struct tm tmDest;
    gmtime_s(&tmDest, &seconds); // Visual C++ only
    // Format struct tm to Cstring
    strftime(time_str_ch, 50, "%a, %d %b %Y %H:%M:%S GMT", &tmDest);
    return std::string(time_str_ch);
}
