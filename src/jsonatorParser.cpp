#include "mblet/jsonator.h"
#include <cstdlib>
#include <cstring>

namespace mblet {

struct JsonatorParseInfo {
    JsonatorParseInfo(const char* filename_, bool additionnalNext_, bool comment_) :
        filename(filename_),
        additionalNext(additionnalNext_),
        comment(comment_)
    {}
    ~JsonatorParseInfo() {}

    inline std::size_t line(std::size_t i) const {
        return indexToLine[i];
    }

    inline std::size_t column(std::size_t i) const {
        return i - lineToIndex[indexToLine[i] - 1];
    }

    inline std::size_t lastLine(std::size_t /*i*/) const {
        return indexToLine.back();
    }

    inline std::size_t lastColumn(std::size_t i) const {
        return i - lineToIndex[indexToLine.back() - 1];
    }

    const char* filename;
    bool additionalNext;
    bool comment;
    std::vector<std::size_t> indexToLine;
    std::vector<std::size_t> lineToIndex;
};

static inline std::string s_replaceEscapeChar(const std::string& str) {
    static const std::pair<char, char> pairChars[] = {
        std::pair<char, char>('a', '\a'), // Alert (bell, alarm)
        std::pair<char, char>('b', '\b'), // Backspace
        std::pair<char, char>('f', '\f'), // Form feed (new page)
        std::pair<char, char>('n', '\n'), // New-line
        std::pair<char, char>('r', '\r'), // Carriage return
        std::pair<char, char>('t', '\t'), // Horizontal tab
        std::pair<char, char>('v', '\v'), // Vertical tab
        std::pair<char, char>('\'', '\''), // Single quotation mark
        std::pair<char, char>('"', '\"'), // Double quotation mark
        std::pair<char, char>('\\', '\\') // Backslash
    };
    static const std::map<char, char> escapeChar(pairChars, pairChars + sizeof(pairChars) / sizeof(pairChars[0]));
    std::string ret(str);
    for (std::size_t i = 0; i < ret.size(); ++i) {
        if (ret[i] == '\\') {
            const std::map<char, char>::const_iterator cit = escapeChar.find(ret[i + 1]);
            if (cit != escapeChar.end()) {
                ret.erase(i, 1);
                ret[i] = cit->second;
            }
        }
    }
    return ret;
}

/**
 * @brief move index to character after spaces
 *
 * @param str
 * @param index
 */
static inline void s_stringJumpSpace(const std::string& str, std::size_t& index) {
    while (::isspace(str[index])) {
        ++index;
    }
}

static void s_parseObject(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator::Map& map);
static void s_parseArray(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator::Map& map);

static inline void s_parseNull(std::size_t& i, Jsonator::Map& map) {
    i += 4;
    // insert value
    map.type = Jsonator::Map::NONE;
    map.str = "null";
    map.number = 0;
    map.boolean = false;
}

static inline void s_parseBool(bool boolean, std::size_t& i, Jsonator::Map& map) {
    if (boolean) {
        i += 4;
        map.str = "true";
    }
    else {
        i += 5;
        map.str = "false";
    }
    map.type = Jsonator::Map::BOOL;
    map.boolean = boolean;
    map.number = 0;
}

static inline void s_parseNumber(const JsonatorParseInfo& info, const std::string& str, std::size_t& i,
                                 Jsonator::Map& map) {
    if (str[i] == '0' && ::isdigit(str[i + 1])) {
        throw Jsonator::ParseException(info.filename, info.line(i), info.column(i), "Octal number not allowed");
    }
    char* ret = NULL;
    map.type = Jsonator::Map::NUMBER;
    map.number = ::strtod(str.c_str() + i, &ret);
    if (ret == NULL) {
        throw Jsonator::ParseException(info.filename, info.line(i), info.column(i), "Bad number");
    }
    map.str = str.substr(i, ret - (str.c_str() + i));
    map.boolean = (map.number != 0.0) ? true : false;
    if (map.str.size() == 0) {
        throw Jsonator::ParseException(info.filename, info.line(i), info.column(i), "Bad number");
    }
    i += map.str.size();
}

static inline void s_parseString(const JsonatorParseInfo& info, const std::string& str, std::size_t& i,
                                 Jsonator::Map& map) {
    ++i; // jump '"'
    std::size_t start = i;
    // search end quote
    while (str[i] != '"') {
        if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
            ++i;
        }
        else if (str[i] == '\0') {
            throw Jsonator::ParseException(info.filename, info.lastLine(i), info.lastColumn(i), "End of string");
        }
        else if (str[i] == '\n') {
            throw Jsonator::ParseException(info.filename, info.line(start), info.column(start), "New line in string");
        }
        ++i;
    }
    std::size_t end = i;
    ++i; // jump '"'
    // insert value
    map.type = Jsonator::Map::STRING;
    map.str = s_replaceEscapeChar(str.substr(start, end - start));
    map.number = 0;
    map.boolean = true;
}

static inline Jsonator::Map& s_createNewObjectElement(const JsonatorParseInfo& info, const std::string& str,
                                                      std::size_t& i, Jsonator::Map& map) {
    // parser key
    ++i; // jump '"'
    std::size_t start = i;
    // search end quote
    while (str[i] != '"') {
        if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
            ++i;
        }
        else if (str[i] == '\0') {
            throw Jsonator::ParseException(info.filename, info.lastLine(i), info.lastColumn(i), "End of key");
        }
        else if (str[i] == '\n') {
            throw Jsonator::ParseException(info.filename, info.line(start), info.column(start), "New line in key");
        }
        ++i;
    }
    std::size_t end = i;
    ++i; // jump '"'
    s_stringJumpSpace(str, i);
    if (str[i] != ':') {
        throw Jsonator::ParseException(info.filename, info.line(i), info.column(i), "Need definition of object");
    }
    ++i; // jump ':'
    s_stringJumpSpace(str, i);
    // get key
    std::string key = s_replaceEscapeChar(str.substr(start, end - start));
    if (map.find(key) != map.end()) {
        throw Jsonator::ParseException(info.filename, info.line(start), info.column(start), "Key already exist");
    }
    // insert key
    Jsonator::Map& keyMap = map.insert(std::pair<std::string, Jsonator::Map>(key, Jsonator::Map())).first->second;
    keyMap.key = key;
    return keyMap;
}

static inline Jsonator::Map& s_createNewTableElement(Jsonator::Map& map) {
    char str[32];
    snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(map.size()));
    Jsonator::Map& retMap = map.insert(std::pair<std::string, Jsonator::Map>(str, Jsonator::Map())).first->second;
    retMap.key = map.key;
    return retMap;
}

static inline bool s_parseType(const JsonatorParseInfo& info, const std::string& str, std::size_t& i,
                               Jsonator::Map& map) {
    switch (str[i]) {
        case '[':
            s_parseArray(info, str, i, map);
            break;
        case '{':
            s_parseObject(info, str, i, map);
            break;
        case '"':
            s_parseString(info, str, i, map);
            break;
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            s_parseNumber(info, str, i, map);
            break;
        default:
            if (!::strncmp(str.c_str() + i, "true", sizeof("true") - 1)) {
                s_parseBool(true, i, map);
            }
            else if (!::strncmp(str.c_str() + i, "false", sizeof("false") - 1)) {
                s_parseBool(false, i, map);
            }
            else if (!::strncmp(str.c_str() + i, "null", sizeof("null") - 1)) {
                s_parseNull(i, map);
            }
            else {
                return false;
            }
            break;
    }
    return true;
}

void s_parseArray(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator::Map& map) {
    map.type = Jsonator::Map::ARRAY;
    map.str = "";
    bool next = false;

    ++i; // jump '['
    s_stringJumpSpace(str, i);
    while (str[i] != ']' || (next == true && info.additionalNext == false)) {
        if (str[i] == '\0') {
            throw Jsonator::ParseException(info.filename, info.lastLine(i), info.lastColumn(i), "End of array not found");
        }
        // search table, object, string, number, bool or null
        if (!s_parseType(info, str, i, s_createNewTableElement(map))) {
            throw Jsonator::ParseException(info.filename, info.line(i), info.column(i), "Bad element of array");
        }
        s_stringJumpSpace(str, i);
        // next
        if (str[i] == ',') {
            ++i; // jump ','
            next = true;
            s_stringJumpSpace(str, i);
        }
        else {
            next = false;
        }
    }
    ++i; // jump ']'
}

void s_parseObject(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator::Map& map) {
    map.type = Jsonator::Map::OBJECT;
    map.str = "";
    bool next = false;

    ++i; // jump '{'
    s_stringJumpSpace(str, i);
    while (str[i] != '}' || (next == true && info.additionalNext == false)) {
        if (str[i] == '\0') {
            throw Jsonator::ParseException(info.filename, info.lastLine(i), info.lastColumn(i), "End of object not found");
        }
        else if (str[i] == '"') {
            // search table, object, string, number, bool or null
            if (!s_parseType(info, str, i, s_createNewObjectElement(info, str, i, map))) {
                throw Jsonator::ParseException(info.filename, info.line(i), info.column(i), "Bad element in the key");
            }
        }
        else {
            throw Jsonator::ParseException(info.filename, info.line(i), info.column(i), "Key of object not found");
        }
        s_stringJumpSpace(str, i);
        // next
        if (str[i] == ',') {
            ++i; // jump ','
            next = true;
            s_stringJumpSpace(str, i);
        }
        else {
            next = false;
        }
    }
    ++i; // jump '}'
}

void Jsonator::_parseStream(std::istream& stream) {
    JsonatorParseInfo info(_filename.c_str(), _parseAdditionalNext, _parseComment);
    _map.clear();
    // generate line and column index
    std::ostringstream oss("");
    std::string line("");
    std::size_t nLine = 1;
    info.lineToIndex.push_back(0);
    while (std::getline(stream, line)) {
        oss << line << '\n';
        info.lineToIndex.push_back(line.size() + 1 + info.lineToIndex.back());
        info.indexToLine.insert(info.indexToLine.end(), line.size() + 1, nLine);
        ++nLine;
    }
    std::string str(oss.str());
    if (info.comment) {
        for (std::size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '"') {
                ++i;
                while (str[i] != '"') {
                    if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
                        ++i;
                    }
                    else if (str[i] == '\n' || str[i] == '\0') {
                        break;
                    }
                    ++i;
                }
            }
            else if (str[i] == '/' && str[i + 1] == '*') {
                str[i] = ' ';
                ++i;
                str[i] = ' ';
                ++i;
                while (str[i] != '*' || str[i + 1] != '/') {
                    if (str[i] == '\0') {
                        break;
                    }
                    str[i] = ' ';
                    ++i;
                }
                if (str[i] != '\0') {
                    str[i] = ' ';
                    str[i + 1] = ' ';
                }
            }
            else if (str[i] == '/' && str[i + 1] == '/') {
                str[i] = ' ';
                ++i;
                str[i] = ' ';
                ++i;
                while (str[i] != '\n') {
                    if (str[i] == '\0') {
                        break;
                    }
                    str[i] = ' ';
                    ++i;
                }
            }
        }
    }
    std::size_t i = 0;
    s_stringJumpSpace(str, i);
    try {
        if (str[i] == '{') {
            s_parseObject(info, str, i, _map);
        }
        else if (str[i] == '[') {
            s_parseArray(info, str, i, _map);
        }
        else if (str[i] == '\0') {
            return;
        }
        else {
            throw ParseException(info.filename, info.line(i), info.column(i), "Not a valid start character");
        }
    }
    catch (const ParseException& /*e*/) {
        _map.clear();
        throw;
    }
}

} // namespace mblet