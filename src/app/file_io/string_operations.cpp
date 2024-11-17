#include "string_operations.h"

#include<algorithm>

std::vector<std::string> StringOperations::split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

std::string StringOperations::remove_chars(std::string s, const std::string& chars) {
    s.erase(std::remove_if(s.begin(), s.end(), [&chars](const char& c) {
        return chars.find(c) != std::string::npos;
    }), s.end());
    return s;
}

