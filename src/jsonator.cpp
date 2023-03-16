/**
 * jsonator.cpp
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2022-2023 BLET Mickaël.
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

#include <ctype.h>  // ::isdigit, ::isspace
#include <stdlib.h> // ::strtod
#include <string.h> // ::strncmp

#include <fstream> // std::ifstream
#include <iomanip> // std::setprecision
#include <limits>  // std::numeric_limits

namespace mblet {

Jsonator::Jsonator() :
    _type(NONE_TYPE) {}

Jsonator::Jsonator(const Jsonator& rhs) :
    _type(rhs._type) {
    switch (_type) {
        case NONE_TYPE:
            break;
        case OBJECT_TYPE:
            _value.object = new std::map<std::string, Jsonator>(*(rhs._value.object));
            break;
        case ARRAY_TYPE:
            _value.array = new std::vector<Jsonator>(*(rhs._value.array));
            break;
        case STRING_TYPE:
            _value.string = new std::string(*(rhs._value.string));
            break;
        case NUMBER_TYPE:
            _value.number = rhs._value.number;
            break;
        case BOOLEAN_TYPE:
            _value.boolean = rhs._value.boolean;
            break;
    }
}

Jsonator::~Jsonator() {
    clear();
}

Jsonator& Jsonator::operator=(const Jsonator& json) {
    if (!isNull()) {
        throw AccessException(*this, "is not null");
    }
    switch (json._type) {
        case NONE_TYPE:
            break;
        case OBJECT_TYPE:
            _value.object = new std::map<std::string, Jsonator>(*(json._value.object));
            break;
        case ARRAY_TYPE:
            _value.array = new std::vector<Jsonator>(*(json._value.array));
            break;
        case STRING_TYPE:
            _value.string = new std::string(*(json._value.string));
            break;
        case NUMBER_TYPE:
            _value.number = json._value.number;
            break;
        case BOOLEAN_TYPE:
            _value.boolean = json._value.boolean;
            break;
    }
    _type = json._type;
    return *this;
}

Jsonator Jsonator::parseFile(const char* filename, bool comment, bool next) {
    std::ifstream fileStream(filename); // open file
    if (fileStream.is_open()) {
        Jsonator json = _parseStream(fileStream, filename, comment, next); // parse file
        fileStream.close();
        return json;
    }
    else {
        throw ParseException(filename, "Open file failed");
    }
}

static inline void s_stringEscape(std::ostream& oss, const std::string& str) {
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

static inline void s_newlineDump(std::ostream& oss, const Jsonator& json, std::size_t indent) {
    if (!json.empty() && indent != 0) {
        oss << '\n';
    }
}

static inline void s_indentDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter,
                                std::size_t index) {
    if (!json.empty() && indent != 0) {
        oss << std::string(indent * index, indentCharacter);
    }
}

static inline void s_nullDump(std::ostream& oss, const Jsonator& /*json*/, std::size_t /*indent*/,
                              char /*indentCharacter*/, std::size_t /*index*/) {
    oss << "null";
}

static inline void s_numberDump(std::ostream& oss, const Jsonator& json, std::size_t /*indent*/,
                                char /*indentCharacter*/, std::size_t /*index*/) {
    oss << json.getNumber();
}

static inline void s_booleanDump(std::ostream& oss, const Jsonator& json, std::size_t /*indent*/,
                                 char /*indentCharacter*/, std::size_t /*index*/) {
    if (json.getBoolean()) {
        oss << "true";
    }
    else {
        oss << "false";
    }
}

static inline void s_stringDump(std::ostream& oss, const Jsonator& json, std::size_t /*indent*/,
                                char /*indentCharacter*/, std::size_t /*index*/) {
    s_stringEscape(oss, json.getString());
}

static void s_typeDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter,
                       std::size_t index = 0);

static void s_objectDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter,
                         std::size_t index) {
    oss << '{';
    s_newlineDump(oss, json, indent);
    ++index;
    for (Jsonator::object_const_iterator cit = json.object_begin(); cit != json.object_end(); ++cit) {
        if (cit != json.object_begin()) {
            oss << ',';
            s_newlineDump(oss, json, indent);
        }
        s_indentDump(oss, json, indent, indentCharacter, index);
        s_stringEscape(oss, cit->first);
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

static void s_arrayDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter,
                        std::size_t index) {
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
        case Jsonator::NONE_TYPE:
            s_nullDump(oss, json, indent, indentCharacter, index);
            break;
        case Jsonator::OBJECT_TYPE:
            s_objectDump(oss, json, indent, indentCharacter, index);
            break;
        case Jsonator::ARRAY_TYPE:
            s_arrayDump(oss, json, indent, indentCharacter, index);
            break;
        case Jsonator::STRING_TYPE:
            s_stringDump(oss, json, indent, indentCharacter, index);
            break;
        case Jsonator::NUMBER_TYPE:
            s_numberDump(oss, json, indent, indentCharacter, index);
            break;
        case Jsonator::BOOLEAN_TYPE:
            s_booleanDump(oss, json, indent, indentCharacter, index);
            break;
    }
}

void Jsonator::dump(std::ostream& os, std::size_t indent, char indentCharacter) const {
    os << std::setprecision(std::numeric_limits<double>::digits10 + 1);
    s_typeDump(os, *this, indent, indentCharacter);
}

std::string Jsonator::dump(std::size_t indent, char indentCharacter) const {
    std::ostringstream oss("");
    dump(oss, indent, indentCharacter);
    return oss.str();
}

Jsonator& Jsonator::clear() {
    switch (_type) {
        case NONE_TYPE:
        case NUMBER_TYPE:
        case BOOLEAN_TYPE:
            break;
        case OBJECT_TYPE:
            delete _value.object;
            break;
        case ARRAY_TYPE:
            delete _value.array;
            break;
        case STRING_TYPE:
            delete _value.string;
            break;
    }
    _type = NONE_TYPE;
    return *this;
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
    if (json.find(key) != json.object_end()) {
        throw Jsonator::ParseException(info.filename, info.line(start), info.column(start), "Key already exist");
    }
    return json[key];
}

static inline Jsonator& s_createNewArrayElement(Jsonator& json) {
    return json[json.size()];
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
    for (std::size_t i = 0; i < str.size() && str[i] != '\0'; ++i) {
        if (str[i] == '"') {
            ++i;
            while (str[i] != '"') {
                if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
                    ++i; // escape character
                }
                if (str[i] == '\n' || str[i] == '\0') {
                    break;
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

void Jsonator::_swap(Jsonator& json) {
    EType typeTmp = _type;
    UValue valueTmp = _value;
    _type = json._type;
    _value = json._value;
    json._type = typeTmp;
    json._value = valueTmp;
}

} // namespace mblet