#ifndef _TEST_BLET_JSON_TO_STRING_H_
#define _TEST_BLET_JSON_TO_STRING_H_

#include <string>

// stringify with erase first and last parenthesis
#define JSON_TO_STRING(j) removeSpaces(std::string(#j + 1, sizeof(#j) - 3))

/**
 * @brief remove all the spaces in string
 *
 * @param str
 * @return std::string
 */
inline std::string removeSpaces(const std::string& str) {
    std::string ret("");
    for (std::size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '"') {
            ret.push_back(str[i]);
            ++i;
            while (str[i] != '\n' && str[i] != '\0' && str[i] != '"') {
                if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
                    ret.push_back(str[i]);
                    ++i; // escape character
                }
                ret.push_back(str[i]);
                ++i;
            }
        }
        if (!::isspace(str[i])) {
            ret.push_back(str[i]);
        }
    }
    return ret;
}

#endif // #ifndef _TEST_BLET_JSON_TO_STRING_H_