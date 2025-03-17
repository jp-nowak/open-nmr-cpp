#ifndef STRING_OPERATIONS_H
#define STRING_OPERATIONS_H

#include <vector>
#include <string>

namespace StringOperations {

std::vector<std::string> split(const std::string& s, const std::string& delimiter);

// returns new string with removed chars (it removes single chars not sequence)
std::string remove_chars(std::string s, const std::string& chars);
}

#endif // STRING_OPERATIONS_H
