/**
 * jsonator.cpp
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2022 BLET Mickaël.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "mblet/jsonator.h"

#include <cctype>  // ::isdigit, ::isspace
#include <cstdlib> // ::strtod
#include <cstring> // ::strncmp
#include <fstream> // std::ifstream
#include <iomanip> // std::setprecision
#include <limits>  // std::numeric_limits

namespace mblet {

Jsonator::Jsonator() :
    std::map<std::string, Jsonator>(),
    _parent(NULL),
    _key(""),
    _string("null"),
    _number(0),
    _boolean(false),
    _type(NONE),
    _filename("") {}

Jsonator::Jsonator(const Jsonator* parent, const std::string& key) :
    std::map<std::string, Jsonator>(),
    _parent(parent),
    _key(key),
    _string("null"),
    _number(0),
    _boolean(false),
    _type(NONE),
    _filename("") {}

Jsonator::Jsonator(const Jsonator& rhs) :
    std::map<std::string, Jsonator>(rhs),
    _parent(rhs._parent),
    _key(rhs._key),
    _string(rhs._string),
    _number(rhs._number),
    _boolean(rhs._boolean),
    _type(rhs._type),
    _filename(rhs._filename) {
    _replaceParent(*this);
}

Jsonator::~Jsonator() {}

void Jsonator::operator=(const Jsonator& json) {
    if (_type != NONE) {
        throw AccessException(*this, "is not null");
    }
    _string = json._string;
    _number = json._number;
    _boolean = json._boolean;
    _type = json._type;
    _filename = json._filename;
    std::map<std::string, Jsonator>::operator=(json);
    _replaceParent(*this);
}

Jsonator Jsonator::parseFile(const char* filename, bool comment, bool next) {
    std::ifstream fileStream(filename); // open file
    if (fileStream.is_open()) {
        Jsonator json = _parseStream(fileStream, filename, comment, next); // parse file
        fileStream.close();
        json._filename = filename;
        return json;
    }
    else {
        throw ParseException(filename, "Open file failed");
    }
}

static inline void s_newlineDump(std::ostream& oss, const Jsonator& json, std::size_t indent) {
    if (!json.empty() && indent != 0) {
        oss << '\n';
    }
}

static inline void s_indentDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter, std::size_t index) {
    if (!json.empty() && indent != 0) {
        oss << std::string(indent * index, indentCharacter);
    }
}

static inline void s_stringDump(std::ostream& oss, const std::string& str) {
    static const std::pair<char, std::string> pairChars[] = {
        std::pair<char, std::string>('\a', "\\a"),  // Alert (bell, alarm)
        std::pair<char, std::string>('\b', "\\b"),  // Backspace
        std::pair<char, std::string>('\f', "\\f"),  // Form feed (new page)
        std::pair<char, std::string>('\n', "\\n"),  // New-line
        std::pair<char, std::string>('\r', "\\r"),  // Carriage return
        std::pair<char, std::string>('\t', "\\t"),  // Horizontal tab
        std::pair<char, std::string>('\v', "\\v"),  // Vertical tab
        std::pair<char, std::string>('\'', "\\'"),  // Single quotation mark
        std::pair<char, std::string>('\"', "\\\""), // Double quotation mark
        std::pair<char, std::string>('\\', "\\\\")  // Backslash
    };
    static const std::map<char, std::string> escapeChar(pairChars, pairChars + sizeof(pairChars) / sizeof(*pairChars));

    oss << '"';
    for (std::size_t i = 0; i < str.size(); ++i) {
        std::map<char, std::string>::const_iterator cit = escapeChar.find(str[i]);
        if (cit != escapeChar.end()) {
            oss << cit->second;
        }
        else {
            oss << str[i];
        }
    }
    oss << '"';
}

static void s_typeDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter, std::size_t index = 0);

static void s_objectDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter, std::size_t index) {
    oss << '{';
    s_newlineDump(oss, json, indent);
    ++index;
    for (Jsonator::const_iterator cit = json.begin(); cit != json.end(); ++cit) {
        if (cit != json.begin()) {
            oss << ',';
            s_newlineDump(oss, json, indent);
        }
        s_indentDump(oss, json, indent, indentCharacter, index);
        s_stringDump(oss, cit->second.getKey()); // key
        oss << ':';
        if (indent != 0) {
            oss << ' ';
        }
        s_typeDump(oss, cit->second, indent, indentCharacter, index);
    }
    s_newlineDump(oss, json, indent);
    --index;
    s_indentDump(oss, json, indent, indentCharacter, index);
    oss << '}';
}

static void s_arrayDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter, std::size_t index) {
    oss << '[';
    s_newlineDump(oss, json, indent);
    ++index;
    for (std::size_t i = 0; i < json.size(); ++i) {
        if (i != 0) {
            oss << ',';
            s_newlineDump(oss, json, indent);
        }
        s_indentDump(oss, json, indent, indentCharacter, index);
        s_typeDump(oss, json[i], indent, indentCharacter, index);
    }
    s_newlineDump(oss, json, indent);
    --index;
    s_indentDump(oss, json, indent, indentCharacter, index);
    oss << ']';
}

void s_typeDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter, std::size_t index) {
    switch (json.getType()) {
        case Jsonator::ARRAY:
            s_arrayDump(oss, json, indent, indentCharacter, index);
            break;
        case Jsonator::OBJECT:
            s_objectDump(oss, json, indent, indentCharacter, index);
            break;
        case Jsonator::STRING:
            s_stringDump(oss, json.getString());
            break;
        case Jsonator::NUMBER:
            oss << json.getNumber();
            break;
        case Jsonator::BOOLEAN:
            if (json.getBoolean()) {
                oss << "true";
            }
            else {
                oss << "false";
            }
            break;
        case Jsonator::NONE:
            oss << "null";
            break;
    }
}

std::string Jsonator::dump(std::size_t indent, char indentCharacter) const {
    std::ostringstream oss("");
    oss << std::setprecision(std::numeric_limits<double>::digits10 + 1);
    s_typeDump(oss, *this, indent, indentCharacter);
    return oss.str();
}

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

/**
 * @brief structure of info parser
 */
struct JsonatorParseInfo {
    JsonatorParseInfo(const std::string& filename_, bool additionnalNext_) :
        filename(filename_),
        additionalNext(additionnalNext_) {}
    ~JsonatorParseInfo() {}

    std::size_t line(std::size_t i) const {
        return indexToLine[i];
    }

    std::size_t column(std::size_t i) const {
        return i - lineToIndex[indexToLine[i] - 1] + 1;
    }

    std::size_t lastLine(std::size_t /*i*/) const {
        return indexToLine.back();
    }

    std::size_t lastColumn(std::size_t i) const {
        return i + 1 - lineToIndex[indexToLine.back() - 1];
    }

    const std::string& filename;
    bool additionalNext;
    std::vector<std::size_t> indexToLine;
    std::vector<std::size_t> lineToIndex;
};

static inline std::string s_replaceEscapeChar(const std::string& str) {
    static const std::pair<char, char> pairChars[] = {
        std::pair<char, char>('a', '\a'),  // Alert (bell, alarm)
        std::pair<char, char>('b', '\b'),  // Backspace
        std::pair<char, char>('f', '\f'),  // Form feed (new page)
        std::pair<char, char>('n', '\n'),  // New-line
        std::pair<char, char>('r', '\r'),  // Carriage return
        std::pair<char, char>('t', '\t'),  // Horizontal tab
        std::pair<char, char>('v', '\v'),  // Vertical tab
        std::pair<char, char>('\'', '\''), // Single quotation mark
        std::pair<char, char>('"', '\"'),  // Double quotation mark
        std::pair<char, char>('\\', '\\')  // Backslash
    };
    static const std::map<char, char> escapeChar(pairChars, pairChars + sizeof(pairChars) / sizeof(*pairChars));
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

static inline void s_parseNull(std::size_t& i, Jsonator& json) {
    json.newNull();
    i += sizeof("null") - 1;
}

static inline void s_parseBool(bool boolean, std::size_t& i, Jsonator& json) {
    json.newBoolean(boolean);
    if (boolean) {
        i += sizeof("true") - 1;
    }
    else {
        i += sizeof("false") - 1;
    }
}

static inline void s_parseNumber(const JsonatorParseInfo& info, const std::string& str, std::size_t& i,
                                 Jsonator& json) {
    if (str[i] == '0' && ::isdigit(str[i + 1])) {
        throw Jsonator::ParseException(info.filename, info.line(i), info.column(i), "Octal number not allowed");
    }
    char* ret = NULL;
    double number = ::strtod(str.c_str() + i, &ret);
    json.newNumber(number);
    std::size_t jump = ret - (str.c_str() + i);
    if (jump == 0) {
        throw Jsonator::ParseException(info.filename, info.line(i), info.column(i), "Bad number");
    }
    i += jump;
}

static inline void s_parseString(const JsonatorParseInfo& info, const std::string& str, std::size_t& i,
                                 Jsonator& json) {
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
    json.newString(s_replaceEscapeChar(str.substr(start, end - start)));
}

static inline Jsonator& s_createNewObjectElement(const JsonatorParseInfo& info, const std::string& str, std::size_t& i,
                                                 Jsonator& json) {
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
    if (json.find(key) != json.end()) {
        throw Jsonator::ParseException(info.filename, info.line(start), info.column(start), "Key already exist");
    }
    return json.std::map<std::string, Jsonator>::insert(std::pair<std::string, Jsonator>(key, Jsonator(&json, key)))
        .first->second;
}

static inline void s_ultostr(char** pstr, unsigned long l) {
    if (l < 10UL) {
        **pstr = '0' + l;
        ++(*pstr);
    }
    else {
        s_ultostr(pstr, l / 10UL);
        **pstr = '0' + (l % 10UL);
        ++(*pstr);
    }
}

static inline std::string s_indexToString(unsigned long index) {
    static const char* numbers[] = {
        "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "10", "11", "12", "13", "14", "15", "16",
        "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33",
        "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50",
        "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65", "66", "67",
        "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83", "84",
        "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99"};
    if (index < 10) {
        return std::string(numbers[index], 1);
    }
    else if (index < 100) {
        return std::string(numbers[index], 2);
    }
    else {
        char str[32];
        char* tmp = str;
        s_ultostr(&tmp, index);
        return std::string(str, tmp - str);
    }
}

static inline Jsonator& s_createNewArrayElement(Jsonator& json) {
    std::string str = s_indexToString(json.size());
    return json.std::map<std::string, Jsonator>::insert(std::pair<std::string, Jsonator>(str, Jsonator(&json, str)))
        .first->second;
}

static bool s_parseType(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator& json);

static void s_parseArray(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator& json) {
    json.newArray();
    bool next = false;

    ++i; // jump '['
    s_stringJumpSpace(str, i);
    while (str[i] != ']' || (next == true && info.additionalNext == false)) {
        if (str[i] == '\0') {
            throw Jsonator::ParseException(info.filename, info.lastLine(i), info.lastColumn(i),
                                           "End of array not found");
        }
        // search array, object, string, number, bool or null
        if (!s_parseType(info, str, i, s_createNewArrayElement(json))) {
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

static void s_parseObject(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator& json) {
    json.newObject();
    bool next = false;

    ++i; // jump '{'
    s_stringJumpSpace(str, i);
    while (str[i] != '}' || (next == true && info.additionalNext == false)) {
        if (str[i] == '\0') {
            throw Jsonator::ParseException(info.filename, info.lastLine(i), info.lastColumn(i),
                                           "End of object not found");
        }
        else if (str[i] == '"') {
            // search array, object, string, number, bool or null
            if (!s_parseType(info, str, i, s_createNewObjectElement(info, str, i, json))) {
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

inline bool s_parseType(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator& json) {
    switch (str[i]) {
        case '[':
            s_parseArray(info, str, i, json);
            break;
        case '{':
            s_parseObject(info, str, i, json);
            break;
        case '"':
            s_parseString(info, str, i, json);
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
            s_parseNumber(info, str, i, json);
            break;
        case 't':
            if (!::strncmp(str.c_str() + i, "true", sizeof("true") - 1)) {
                s_parseBool(true, i, json);
            }
            else {
                return false;
            }
            break;
        case 'f':
            if (!::strncmp(str.c_str() + i, "false", sizeof("false") - 1)) {
                s_parseBool(false, i, json);
            }
            else {
                return false;
            }
            break;
        case 'n':
            if (!::strncmp(str.c_str() + i, "null", sizeof("null") - 1)) {
                s_parseNull(i, json);
            }
            else {
                return false;
            }
            break;
        default:
            return false;
            break;
    }
    return true;
}

static void s_replaceCommentBySpace(std::string& str) {
    for (std::size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '"') {
            ++i;
            while (str[i] != '\n' && str[i] != '\0' && str[i] != '"') {
                if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
                    ++i; // escape character
                }
                ++i;
            }
        }
        else if (str[i] == '/' && str[i + 1] == '*') {
            str[i] = ' '; // replace '/'
            ++i;
            str[i] = ' '; // replace '*'
            ++i;
            while (str[i] != '\0' && (str[i] != '*' || str[i + 1] != '/')) {
                str[i] = ' ';
                ++i;
            }
            if (str[i] != '\0') {
                str[i] = ' '; // replace '*'
                ++i;
                str[i] = ' '; // replace '/'
            }
        }
        else if (str[i] == '/' && str[i + 1] == '/') {
            str[i] = ' ';
            ++i;
            str[i] = ' ';
            ++i;
            while (str[i] != '\0' && str[i] != '\n') {
                str[i] = ' ';
                ++i;
            }
        }
    }
}

static std::string s_streamToStr(JsonatorParseInfo& info, std::istream& stream) {
    // generate line and column index
    std::ostringstream oss("");
    std::string line("");
    std::size_t nLine = 1;
    info.lineToIndex.push_back(0);
    while (std::getline(stream, line)) {
        if (nLine > 1) {
            oss << '\n';
        }
        oss << line;
        info.lineToIndex.push_back(line.size() + 1 + info.lineToIndex.back());
        info.indexToLine.insert(info.indexToLine.end(), line.size() + 1, nLine);
        ++nLine;
    }
    return oss.str();
}

Jsonator Jsonator::_parseStream(std::istream& stream, const std::string& filename, bool comment, bool additionalNext) {
    JsonatorParseInfo info(filename, additionalNext);
    std::string str = s_streamToStr(info, stream);
    if (comment) {
        s_replaceCommentBySpace(str);
    }
    std::size_t i = 0;
    s_stringJumpSpace(str, i);
    try {
        Jsonator json;
        switch (str[i]) {
            case '{':
                s_parseObject(info, str, i, json);
                s_stringJumpSpace(str, i);
                break;
            case '[':
                s_parseArray(info, str, i, json);
                s_stringJumpSpace(str, i);
                break;
            case '\0':
                return json;
            default:
                throw ParseException(info.filename, info.line(i), info.column(i), "Not a valid start character");
        }
        if (str[i] != '\0') {
            throw ParseException(info.filename, info.line(i), info.column(i), "Not a valid end character");
        }
        return json;
    }
    catch (const ParseException& /*e*/) {
        throw;
    }
}

void Jsonator::_replaceParent(Jsonator& json) {
    for (Jsonator::iterator it = json.begin(); it != json.end(); ++it) {
        it->second._parent = &json;
        _replaceParent(it->second);
    }
}

void Jsonator::_replace(const Jsonator& json) {
    _key = json._key;
    _string = json._string;
    _number = json._number;
    _boolean = json._boolean;
    _type = json._type;
    _filename = json._filename;
    std::map<std::string, Jsonator>::operator=(json);
    _replaceParent(*this);
}

} // namespace mblet