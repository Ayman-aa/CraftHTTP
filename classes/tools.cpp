#include "../includes/tools.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

bool isAllSpaces(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!std::isspace(*it)) {
            return false;
        }
    }
    return true;
}

bool isAllDigit(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!std::isdigit(*it)) {
            return false;
        }
    }
    return true;
}

bool allCharactersAllowed(const std::string& str, const std::string& allowedChars) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (allowedChars.find(*it) == std::string::npos) {
            return false;
        }
    }
    return true;
}

std::string toString(int n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}

std::string getFileExtension(const std::string& filePath)
{
    size_t pos = filePath.find_last_of(".");
    return (pos == std::string::npos) ? "" : filePath.substr(pos + 1);
}

std::vector<std::string> splitWithDelimiters(const std::string& str, const std::vector<std::string>& delimiters) {
    std::vector<std::string> result;
    size_t start = 0;

    while (start < str.size()) {
        size_t minPos = std::string::npos;
        std::string minDelimiter;

        for (std::vector<std::string>::const_iterator it = delimiters.begin(); it != delimiters.end(); ++it) {
            size_t pos = str.find(*it, start);
            if (pos < minPos) {
                minPos = pos;
                minDelimiter = *it;
            }
        }

        if (minPos == std::string::npos) {
            result.push_back(str.substr(start));
            break;
        }

        result.push_back(str.substr(start, minPos - start));
        start = minPos + minDelimiter.size();
    }

    return result;
}

std::vector<std::string> strSplit(const std::string& input, const std::string& delimiters, bool includeEmpty) {
    std::vector<std::string> result;
    size_t start = 0, end = 0;

    while ((end = input.find_first_of(delimiters, start)) != std::string::npos) {
        if (includeEmpty || (end - start > 0)) {
            result.push_back(input.substr(start, end - start));
        }
        start = end + 1;
    }

    if (includeEmpty || (input.length() - start > 0)) {
        result.push_back(input.substr(start));
    }

    return result;
}

std::pair<std::string, std::string> split(const std::string& str, char sep) {
    size_t pos = str.find(sep);
    if (pos == std::string::npos) {
        return std::make_pair(str, "");
    }
    return std::make_pair(str.substr(0, pos), str.substr(pos + 1));
}

std::string strtrim(const std::string& str, const std::string& charsToTrim) {
    size_t first = str.find_first_not_of(charsToTrim);
    if (first == std::string::npos) {
        // String is all whitespace
        return "";
    }

    size_t last = str.find_last_not_of(charsToTrim);
    return str.substr(first, last - first + 1);
}

void trim_spaces(std::string& str, bool both) {
    if (both) {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    }
    str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
}

bool fileExists(const std::string& name) {
    std::ifstream file(name.c_str());
    return file.good();
}

std::string generateRandomString(size_t length) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    std::string randomString;
    for (size_t i = 0; i < length; ++i) {
        randomString += charset[rand() % max_index];
    }
    return randomString;
}

std::string generateUniqueFileName(const std::string& directory, const std::string& extension) {
    std::ostringstream ss;
    std::string fileName;

    do {
        ss.str("");
        ss.clear();
        ss << directory << '/' << generateRandomString(12) << extension;
        fileName = ss.str();
    } while (fileExists(fileName));

    return fileName;
}

std::string urlDecode(const std::string& input) {
    std::string result;
    result.reserve(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '%' && i + 2 < input.size()) {
            int hexValue;
            if (sscanf(input.substr(i + 1, 2).c_str(), "%x", &hexValue) == 1) {
                result += static_cast<char>(hexValue);
                i += 2;
            } else {
                result += input[i];
            }
        } else if (input[i] == '+') {
            result += ' ';
        } else {
            result += input[i];
        }
    }

    return result;
}

std::string sanitizeFilename(const std::string& filename) {
    std::string sanitized = filename;
    sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '/'), sanitized.end());
    sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '\\'), sanitized.end());
    return sanitized;
}

// std::string sanitizeFilename(const std::string& filename) {
//     std::string sanitized = filename;
    
//     // Remove path traversal sequences
//     size_t pos = 0;
//     while ((pos = sanitized.find("../", pos)) != std::string::npos) {
//         sanitized.replace(pos, 3, "");
//     }
    
//     // Replace slashes with underscores
//     pos = 0;
//     while ((pos = sanitized.find('/', pos)) != std::string::npos) {
//         sanitized.replace(pos, 1, "_");
//     }
    
//     return sanitized;
// }