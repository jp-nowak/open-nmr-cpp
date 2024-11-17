#ifndef STRING_OPERATIONS_H
#define STRING_OPERATIONS_H

#include <vector>
#include <string>

namespace StringOperations {
    std::vector<std::string> split(std::string s, std::string delimiter);
    std::string remove_chars(std::string s, const std::string& chars);
}

#endif // STRING_OPERATIONS_H
