#include "jsonator.h"
#include <list>
#include <queue>
#include <cstdlib>

namespace mblet {

Jsonator::Map::Map() :
    std::map<std::string, Map>(),
    key(""),
    str(""),
    number(0),
    boolean(false),
    type(NONE)
 {}

Jsonator::Jsonator() {
}

Jsonator::~Jsonator() {
}

Jsonator::Jsonator(const Jsonator& /*rhs*/) {
}

Jsonator& Jsonator::operator=(const Jsonator& rhs) {
    if (this == &rhs)
        return *this;
    return *this;
}

static void s_newlineDump(std::ostream& oss, std::size_t indent) {
    if (indent > 0) {
        oss << '\n';
    }
}

static void s_stringDump(std::ostream& oss, const std::string& str) {
    static const std::pair<char, std::string> pairChars[] = {
        std::pair<char, std::string>('\a', "\\a"), // Alert (bell, alarm)
        std::pair<char, std::string>('\b', "\\b"), // Backspace
        std::pair<char, std::string>('\f', "\\f"), // Form feed (new page)
        std::pair<char, std::string>('\n', "\\n"), // New-line
        std::pair<char, std::string>('\r', "\\r"), // Carriage return
        std::pair<char, std::string>('\t', "\\t"), // Horizontal tab
        std::pair<char, std::string>('\v', "\\v"), // Vertical tab
        std::pair<char, std::string>('\'', "\\'"), // Single quotation mark
        std::pair<char, std::string>('\"', "\\\""), // Double quotation mark
        std::pair<char, std::string>('\?', "\\?"), // Question mark
        std::pair<char, std::string>('\\', "\\\\") // Backslash
    };
    static const std::map<char, std::string> escapeChar(pairChars, pairChars + sizeof(pairChars) / sizeof(pairChars[0]));
    for (std::size_t i = 0; i < str.size(); ++i) {
        const std::map<char, std::string>::const_iterator cit = escapeChar.find(str[i]);
        if (cit != escapeChar.end()) {
            oss << cit->second;
        }
        else {
            oss << str[i];
        }
    }
}

static void s_objectDump(std::ostream& oss, const Jsonator::Map& map, std::size_t indent, std::size_t index) {
    if (map.type == Jsonator::Map::OBJECT) {
        oss << '{';
        if (map.size() > 0) {
            s_newlineDump(oss, indent);
        }
    }
    else if (map.type == Jsonator::Map::TABLE) {
        oss << '[';
        if (map.size() > 0) {
            s_newlineDump(oss, indent);
        }
    }
    ++index;
    Jsonator::Map::const_iterator cit;
    for (cit = map.begin(); cit != map.end(); ++cit) {
        if (cit != map.begin()) {
            oss << ',';
            s_newlineDump(oss, indent);
        }
        if (map.type != Jsonator::Map::TABLE) {
            oss << std::string(indent * index, ' ') << '"';
            s_stringDump(oss, cit->first);
            oss << '"' << ':';
            if (indent > 0) {
                oss << ' ';
            }
        }
        if (map.type == Jsonator::Map::TABLE) {
            oss << std::string(indent * index, ' ');
        }
        if (cit->second.type == Jsonator::Map::OBJECT || cit->second.type == Jsonator::Map::TABLE) {
            s_objectDump(oss, cit->second, indent, index);
        }
        if (cit->second.type == Jsonator::Map::STRING) {
            oss << '"';
            s_stringDump(oss, cit->second.str);
            oss << '"';
        }
        else {
            oss << cit->second.str;
        }
    }
    if (map.size() > 0) {
        s_newlineDump(oss, indent);
    }
    --index;
    if (map.type == Jsonator::Map::OBJECT) {
        if (map.size() > 0) {
            oss << std::string(indent * index, ' ');
        }
        oss << '}';
    }
    if (map.type == Jsonator::Map::TABLE) {
        if (map.size() > 0) {
            oss << std::string(indent * index, ' ');
        }
        oss << ']';
    }
}

static void s_recurseDump(std::ostream& oss, const Jsonator::Map& map, std::size_t indent, std::size_t index = 0) {
    if (map.type == Jsonator::Map::OBJECT) {
        oss << std::string(indent * index, ' ') << '{';
        s_newlineDump(oss, indent);
    }
    else if (map.type == Jsonator::Map::TABLE) {
        oss << std::string(indent * index, ' ') << '[';
        s_newlineDump(oss, indent);
    }
    ++index;
    Jsonator::Map::const_iterator cit;
    for (cit = map.begin(); cit != map.end(); ++cit) {
        if (cit != map.begin()) {
            oss << ',';
            s_newlineDump(oss, indent);
        }
        if (map.type != Jsonator::Map::TABLE) {
            oss << std::string(indent * index, ' ') << '"';
            s_stringDump(oss, cit->first);
            oss << '"' << ':';
            if (indent > 0) {
                oss << ' ';
            }
        }
        if (map.type == Jsonator::Map::TABLE) {
            oss << std::string(indent * index, ' ');
        }
        if (cit->second.type == Jsonator::Map::OBJECT || cit->second.type == Jsonator::Map::TABLE) {
            s_objectDump(oss, cit->second, indent, index);
        }
        if (cit->second.type == Jsonator::Map::STRING) {
            oss << '"';
            s_stringDump(oss, cit->second.str);
            oss << '"';
        }
        else {
            oss << cit->second.str;
        }
    }
    --index;
    s_newlineDump(oss, indent);
    if (map.type == Jsonator::Map::OBJECT) {
        oss << std::string(indent * index, ' ') << '}';
    }
    if (map.type == Jsonator::Map::TABLE) {
        oss << std::string(indent * index, ' ') << ']';
    }
}

std::ostream& Jsonator::dump(std::size_t indent, std::ostream& oss) const {
    s_recurseDump(oss, _map, indent);
    return oss;
}

////////////////////////////////////////////////////////////////////////////////
// PARSER
////////////////////////////////////////////////////////////////////////////////

static std::vector<std::size_t> indexToLine;
static std::vector<std::size_t> lineToIndex;

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
        std::pair<char, char>('?', '\?'), // Question mark
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

static void s_parseObject(const std::string& str, std::size_t& i, Jsonator::Map& map);
static void s_parseTable(const std::string& str, std::size_t& i, Jsonator::Map& map);

static inline void s_parseNull(const std::string& /*str*/, std::size_t& i, Jsonator::Map& map) {
    i += 4;
    // insert value
    map.type = Jsonator::Map::NONE;
    map.str = "null";
    map.number = 0;
    map.boolean = false;
}

static inline void s_parseFalse(const std::string& /*str*/, std::size_t& i, Jsonator::Map& map) {
    i += 5;
    // insert value
    map.type = Jsonator::Map::BOOL;
    map.str = "false";
    map.number = 0;
    map.boolean = false;
}

static inline void s_parseTrue(const std::string& /*str*/, std::size_t& i, Jsonator::Map& map) {
    i += 4;
    // insert value
    map.type = Jsonator::Map::BOOL;
    map.str = "true";
    map.number = 0;
    map.boolean = true;
}

static inline void s_parseNumber(const std::string& str, std::size_t& i, Jsonator::Map& map) {
    if (str[i] == '0' && ::isdigit(str[i + 1])) {
        throw Jsonator::ParseException(indexToLine[i], i - lineToIndex[indexToLine[i] - 1], "Octal number not allowed");
    }
    char* ret = NULL;
    map.type = Jsonator::Map::NUMBER;
    map.number = ::strtod(str.c_str() + i, &ret);
    if (ret == NULL) {
        throw Jsonator::ParseException(indexToLine[i], i - lineToIndex[indexToLine[i] - 1], "Bad number");
    }
    map.str = str.substr(i, ret - (str.c_str() + i));
    map.boolean = (map.number != 0.0) ? true : false;
    if (map.str.size() == 0) {
        throw Jsonator::ParseException(indexToLine[i], i - lineToIndex[indexToLine[i] - 1], "Bad number");
    }
    i += map.str.size();
}

static inline void s_parseString(const std::string& str, std::size_t& i, Jsonator::Map& map) {
    ++i; // jump '"'
    std::size_t start = i;
    // search end quote
    while (str[i] != '"') {
        if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
            ++i;
        }
        else if (str[i] == '\0') {
            throw Jsonator::ParseException(indexToLine.back(), i - lineToIndex[indexToLine.back() - 1], "End of key.");
        }
        else if (str[i] == '\n') {
            throw Jsonator::ParseException(indexToLine[i], i - lineToIndex[indexToLine[i] - 1], "New line in string.");
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

static inline Jsonator::Map& s_createNewTableElement(Jsonator::Map& map) {
    char str[32];
    snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(map.size()));
    Jsonator::Map& retMap = map.insert(std::pair<std::string, Jsonator::Map>(str, Jsonator::Map())).first->second;
    retMap.key = map.key;
    return retMap;
}

void s_parseTable(const std::string& str, std::size_t& i, Jsonator::Map& map) {
    map.type = Jsonator::Map::TABLE;
    bool next = false;

    ++i; // jump '['
    s_stringJumpSpace(str, i);
    while (str[i] != ']' || next == true) {
        if (str[i] == '\0') {
            throw Jsonator::ParseException(indexToLine.back(), i - lineToIndex[indexToLine.back() - 1], "End of table not found.");
        }
        // search table, object, string, number, bool or null
        else if (str[i] == '[') {
            s_parseTable(str, i, s_createNewTableElement(map));
        }
        else if (str[i] == '{') {
            s_parseObject(str, i, s_createNewTableElement(map));
        }
        else if (str[i] == '"') {
            s_parseString(str, i, s_createNewTableElement(map));
        }
        else if (::isdigit(str[i])) {
            s_parseNumber(str, i, s_createNewTableElement(map));
        }
        else if (str[i] == 't' && str[i + 1] == 'r' && str[i + 2] == 'u' && str[i + 3] == 'e')  {
            s_parseTrue(str, i, s_createNewTableElement(map));
        }
        else if(str[i] == 'f' && str[i + 1] == 'a' && str[i + 2] == 'l' && str[i + 3] == 's' && str[i + 4] == 'e') {
            s_parseFalse(str, i, s_createNewTableElement(map));
        }
        else if (str[i] == 'n' && str[i + 1] == 'u' && str[i + 2] == 'l' && str[i + 3] == 'l') {
            s_parseNull(str, i, s_createNewTableElement(map));
        }
        else {
            throw Jsonator::ParseException(indexToLine[i], i - lineToIndex[indexToLine[i] - 1], "Bad element of table.");
        }
        s_stringJumpSpace(str, i);
        // next
        if (str[i] == ',') {
            ++i; // jump ','
            next = true;
        }
        else {
            next = false;
        }
        s_stringJumpSpace(str, i);
    }
    ++i; // jump ']'
}

void s_parseObject(const std::string& str, std::size_t& i, Jsonator::Map& map) {
    map.type = Jsonator::Map::OBJECT;
    bool next = false;

    ++i; // jump '{'
    s_stringJumpSpace(str, i);
    while (str[i] != '}' || next == true) {
        if (str[i] == '\0') {
            throw Jsonator::ParseException(indexToLine.back(), i - lineToIndex[indexToLine.back() - 1], "End of object not found.");
        }
        else if (str[i] == '"') {
            // parse key
            ++i; // jump '"'
            std::size_t start = i;
            // search end quote
            while (str[i] != '"') {
                if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
                    ++i;
                }
                else if (str[i] == '\0') {
                    throw Jsonator::ParseException(indexToLine.back(), i - lineToIndex[indexToLine.back() - 1], "End of key.");
                }
                else if (str[i] == '\n') {
                    throw Jsonator::ParseException(indexToLine[i], i - lineToIndex[indexToLine[i] - 1], "New line in key.");
                }
                ++i;
            }
            std::size_t end = i;
            ++i; // jump '"'
            s_stringJumpSpace(str, i);
            if (str[i] != ':') {
                throw Jsonator::ParseException(indexToLine[i], i - lineToIndex[indexToLine[i] - 1], "Need definition of object.");
            }
            ++i; // jump ':'
            s_stringJumpSpace(str, i);
            // get key
            std::string key = s_replaceEscapeChar(str.substr(start, end - start));
            if (map.find(key) != map.end()) {
                throw Jsonator::ParseException(indexToLine[start], start - lineToIndex[indexToLine[start] - 1], "Key already exist.");
            }
            // insert key
            Jsonator::Map& keyMap = map.insert(std::pair<std::string, Jsonator::Map>(std::string(key), Jsonator::Map())).first->second;
            keyMap.key = key;
            if (str[i] == '[') {
                s_parseTable(str, i, keyMap);
            }
            else if (str[i] == '{') {
                s_parseObject(str, i, keyMap);
            }
            else if (str[i] == '"') {
                s_parseString(str, i, keyMap);
            }
            else if (::isdigit(str[i])) {
                s_parseNumber(str, i, keyMap);
            }
            else if (str[i] == 't' && str[i + 1] == 'r' && str[i + 2] == 'u' && str[i + 3] == 'e')  {
                s_parseTrue(str, i, keyMap);
            }
            else if(str[i] == 'f' && str[i + 1] == 'a' && str[i + 2] == 'l' && str[i + 3] == 's' && str[i + 4] == 'e') {
                s_parseFalse(str, i, keyMap);
            }
            else if (str[i] == 'n' && str[i + 1] == 'u' && str[i + 2] == 'l' && str[i + 3] == 'l') {
                s_parseNull(str, i, keyMap);
            }
            else {
                throw Jsonator::ParseException(indexToLine[i], i - lineToIndex[indexToLine[i] - 1], "Bad element in the key.");
            }
        }
        else {
            throw Jsonator::ParseException(indexToLine[i], i - lineToIndex[indexToLine[i] - 1], "Key of object not found.");
        }
        s_stringJumpSpace(str, i);
        // next
        if (str[i] == ',') {
            ++i; // jump ','
            next = true;
        }
        else {
            next = false;
        }
        s_stringJumpSpace(str, i);
    }
    ++i; // jump '}'
}

void Jsonator::parseStream(std::istream& stream) {
    _map.clear();
    try {
        indexToLine.clear();
        lineToIndex.clear();
        std::ostringstream oss("");
        std::string line("");
        std::size_t nLine = 1;
        lineToIndex.push_back(0);
        while (std::getline(stream, line)) {
            oss << line << '\n';
            lineToIndex.push_back(line.size() + 1 + lineToIndex.back());
            indexToLine.insert(indexToLine.end(), line.size() + 1, nLine);
            ++nLine;
        }
        const std::string str(oss.str());
        std::size_t i = 0;
        s_stringJumpSpace(str, i);
        if (str[i] == '{') {
            s_parseObject(str, i , _map);
        }
        else if (str[i] == '[') {
            s_parseTable(str, i , _map);
        }
        else if (str[i] == '\0') {
            return;
        }
        else {
            throw ParseException(indexToLine[i], i - lineToIndex[indexToLine[i] - 1], "Not a valid start character");
        }
    }
    catch (const ParseException& /*e*/) {
        _map.clear();
        throw;
    }
}

} // namespace mblet