/**
 * json.cpp
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

#include "blet/json.h"

#include <ctype.h>  // ::isdigit, ::isspace
#include <stdlib.h> // ::strtod

#include <fstream> // std::ifstream
#include <iomanip> // std::setprecision
#include <limits>  // std::numeric_limits

namespace blet {

namespace json {

static inline void s_stringEscape(std::ostream& oss, const std::string& str) {
    for (std::size_t i = 0; i < str.size(); ++i) {
        switch (str[i]) {
            case '\a':
                oss << '\\' << 'a';
                break;
            case '\b':
                oss << '\\' << 'b';
                break;
            case '\f':
                oss << '\\' << 'f';
                break;
            case '\n':
                oss << '\\' << 'n';
                break;
            case '\r':
                oss << '\\' << 'r';
                break;
            case '\t':
                oss << '\\' << 't';
                break;
            case '\v':
                oss << '\\' << 'v';
                break;
            case '\'':
                oss << '\\' << '\'';
                break;
            case '"':
                oss << '\\' << '"';
                break;
            case '\\':
                oss << '\\' << '\\';
                break;
            default:
                oss << str[i];
                break;
        }
    }
}

static inline void s_newlineDump(std::ostream& oss, const blet::Dict& dict, std::size_t indent) {
    if (indent != 0) {
        switch (dict.getType()) {
            case blet::Dict::NULL_TYPE:
                break;
            case blet::Dict::OBJECT_TYPE:
                if (!dict.getValue().getObject().empty()) {
                    oss << '\n';
                }
                break;
            case blet::Dict::ARRAY_TYPE:
                if (!dict.getValue().getArray().empty()) {
                    oss << '\n';
                }
                break;
            case blet::Dict::STRING_TYPE:
                break;
            case blet::Dict::NUMBER_TYPE:
                break;
            case blet::Dict::BOOLEAN_TYPE:
                break;
        }
    }
}

static inline void s_indentDump(std::ostream& oss, const blet::Dict& dict, std::size_t indent, char indentCharacter,
                                std::size_t index) {
    if (indent != 0) {
        switch (dict.getType()) {
            case blet::Dict::NULL_TYPE:
                break;
            case blet::Dict::OBJECT_TYPE:
                oss << std::string(indent * index, indentCharacter);
                break;
            case blet::Dict::ARRAY_TYPE:
                oss << std::string(indent * index, indentCharacter);
                break;
            case blet::Dict::STRING_TYPE:
                break;
            case blet::Dict::NUMBER_TYPE:
                break;
            case blet::Dict::BOOLEAN_TYPE:
                break;
        }
    }
}

static inline void s_nullDump(std::ostream& oss, const blet::Dict& /*json*/, std::size_t /*indent*/,
                              char /*indentCharacter*/, std::size_t /*index*/) {
    oss.write("null", sizeof("null") - 1);
}

static inline void s_numberDump(std::ostream& oss, const blet::Dict& dict, std::size_t /*indent*/,
                                char /*indentCharacter*/, std::size_t /*index*/) {
    oss << dict.getNumber();
}

static inline void s_booleanDump(std::ostream& oss, const blet::Dict& dict, std::size_t /*indent*/,
                                 char /*indentCharacter*/, std::size_t /*index*/) {
    if (dict.getBoolean()) {
        oss.write("true", sizeof("true") - 1);
    }
    else {
        oss.write("false", sizeof("false") - 1);
    }
}

static inline void s_stringDump(std::ostream& oss, const blet::Dict& dict, std::size_t /*indent*/,
                                char /*indentCharacter*/, std::size_t /*index*/) {
    oss << '"';
    s_stringEscape(oss, dict.getString());
    oss << '"';
}

static void s_typeDump(std::ostream& oss, const blet::Dict& dict, std::size_t indent, char indentCharacter,
                       std::size_t index = 0);

static void s_objectDump(std::ostream& oss, const blet::Dict& dict, std::size_t indent, char indentCharacter,
                         std::size_t index) {
    oss << '{';
    s_newlineDump(oss, dict, indent);
    ++index;
    for (blet::Dict::object_t::const_iterator cit = dict.getValue().getObject().begin(); cit != dict.getValue().getObject().end();
         ++cit) {
        if (cit != dict.getValue().getObject().begin()) {
            oss << ',';
            s_newlineDump(oss, dict, indent);
        }
        s_indentDump(oss, dict, indent, indentCharacter, index);
        oss << '"';
        s_stringEscape(oss, cit->first);
        oss << '"';
        oss << ':';
        if (indent != 0) {
            oss << ' ';
        }
        s_typeDump(oss, cit->second, indent, indentCharacter, index);
    }
    s_newlineDump(oss, dict, indent);
    --index;
    s_indentDump(oss, dict, indent, indentCharacter, index);
    oss << '}';
}

static void s_arrayDump(std::ostream& oss, const blet::Dict& dict, std::size_t indent, char indentCharacter,
                        std::size_t index) {
    oss << '[';
    s_newlineDump(oss, dict, indent);
    ++index;
    for (std::size_t i = 0; i < dict.getValue().getArray().size(); ++i) {
        if (i != 0) {
            oss << ',';
            s_newlineDump(oss, dict, indent);
        }
        s_indentDump(oss, dict, indent, indentCharacter, index);
        s_typeDump(oss, dict.getValue().getArray()[i], indent, indentCharacter, index);
    }
    s_newlineDump(oss, dict, indent);
    --index;
    s_indentDump(oss, dict, indent, indentCharacter, index);
    oss << ']';
}

void s_typeDump(std::ostream& oss, const blet::Dict& dict, std::size_t indent, char indentCharacter,
                std::size_t index) {
    switch (dict.getType()) {
        case blet::Dict::NULL_TYPE:
            s_nullDump(oss, dict, indent, indentCharacter, index);
            break;
        case blet::Dict::OBJECT_TYPE:
            s_objectDump(oss, dict, indent, indentCharacter, index);
            break;
        case blet::Dict::ARRAY_TYPE:
            s_arrayDump(oss, dict, indent, indentCharacter, index);
            break;
        case blet::Dict::STRING_TYPE:
            s_stringDump(oss, dict, indent, indentCharacter, index);
            break;
        case blet::Dict::NUMBER_TYPE:
            s_numberDump(oss, dict, indent, indentCharacter, index);
            break;
        case blet::Dict::BOOLEAN_TYPE:
            s_booleanDump(oss, dict, indent, indentCharacter, index);
            break;
    }
}

void dump(const blet::Dict& dict, std::ostream& os, std::size_t indent, char indentCharacter) {
    os << std::setprecision(std::numeric_limits<double>::digits10 + 1);
    s_typeDump(os, dict, indent, indentCharacter);
}

std::string dump(const blet::Dict& dict, std::size_t indent, char indentCharacter) {
    std::ostringstream oss("");
    dump(dict, oss, indent, indentCharacter);
    return oss.str();
}

/**
 * @brief structure of info parser
 */
struct JsonParseInfo {
    JsonParseInfo(const std::string& filename_, bool additionnalNext_) :
        filename(filename_),
        additionalNext(additionnalNext_) {}
    ~JsonParseInfo() {}

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
    std::string ret(str);
    for (std::size_t i = 0; i < ret.size(); ++i) {
        if (ret[i] == '\\') {
            switch (ret[i + 1]) {
                case 'a':
                    ret.erase(i, 1);
                    ret[i] = '\a';
                    break;
                case 'b':
                    ret.erase(i, 1);
                    ret[i] = '\b';
                    break;
                case 'f':
                    ret.erase(i, 1);
                    ret[i] = '\f';
                    break;
                case 'n':
                    ret.erase(i, 1);
                    ret[i] = '\n';
                    break;
                case 'r':
                    ret.erase(i, 1);
                    ret[i] = '\r';
                    break;
                case 't':
                    ret.erase(i, 1);
                    ret[i] = '\t';
                    break;
                case 'v':
                    ret.erase(i, 1);
                    ret[i] = '\v';
                    break;
                case '\'':
                    ret.erase(i, 1);
                    ret[i] = '\'';
                    break;
                case '"':
                    ret.erase(i, 1);
                    ret[i] = '\"';
                    break;
                case '\\':
                    ret.erase(i, 1);
                    ret[i] = '\\';
                    break;
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

static inline void s_parseNull(std::size_t& i, blet::Dict& dict) {
    dict.newNull();
    i += sizeof("null") - 1;
}

static inline void s_parseBool(bool boolean, std::size_t& i, blet::Dict& dict) {
    dict.newBoolean(boolean);
    if (boolean) {
        i += sizeof("true") - 1;
    }
    else {
        i += sizeof("false") - 1;
    }
}

static inline void s_parseNumber(const JsonParseInfo& info, const std::string& str, std::size_t& i, blet::Dict& dict) {
    if (str[i] == '0' && ::isdigit(str[i + 1])) {
        throw ParseException(info.filename, info.line(i), info.column(i), "Octal number not allowed");
    }
    char* ret = NULL;
    double number = ::strtod(str.c_str() + i, &ret);
    dict.newNumber(number);
    std::size_t jump = ret - (str.c_str() + i);
    if (jump == 0) {
        throw ParseException(info.filename, info.line(i), info.column(i), "Bad number");
    }
    i += jump;
}

static inline void s_parseString(const JsonParseInfo& info, const std::string& str, std::size_t& i, blet::Dict& dict) {
    ++i; // jump '"'
    std::size_t start = i;
    // search end quote
    while (str[i] != '"') {
        if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
            ++i;
        }
        else if (str[i] == '\0') {
            throw ParseException(info.filename, info.lastLine(i), info.lastColumn(i), "End of string");
        }
        else if (str[i] == '\n') {
            throw ParseException(info.filename, info.line(start), info.column(start), "New line in string");
        }
        ++i;
    }
    std::size_t end = i;
    ++i; // jump '"'
    dict.newString(s_replaceEscapeChar(str.substr(start, end - start)));
}

static inline blet::Dict& s_createNewObjectElement(const JsonParseInfo& info, const std::string& str, std::size_t& i,
                                                   blet::Dict& dict) {
    // parser key
    ++i; // jump '"'
    std::size_t start = i;
    // search end quote
    while (str[i] != '"') {
        if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
            ++i;
        }
        else if (str[i] == '\0') {
            throw ParseException(info.filename, info.lastLine(i), info.lastColumn(i), "End of key");
        }
        else if (str[i] == '\n') {
            throw ParseException(info.filename, info.line(start), info.column(start), "New line in key");
        }
        ++i;
    }
    std::size_t end = i;
    ++i; // jump '"'
    s_stringJumpSpace(str, i);
    if (str[i] != ':') {
        throw ParseException(info.filename, info.line(i), info.column(i), "Need definition of object");
    }
    ++i; // jump ':'
    s_stringJumpSpace(str, i);
    // get key
    std::string key = s_replaceEscapeChar(str.substr(start, end - start));
    if (dict.getValue().getObject().find(key) != dict.getValue().getObject().end()) {
        throw ParseException(info.filename, info.line(start), info.column(start), "Key already exist");
    }
    return dict.getValue().getObject()[key];
}

static inline blet::Dict& s_createNewArrayElement(blet::Dict& dict) {
    dict.getValue().getArray().push_back(blet::Dict());
    return dict.getValue().getArray().back();
}

static bool s_parseType(const JsonParseInfo& info, const std::string& str, std::size_t& i, blet::Dict& dict);

static void s_parseArray(const JsonParseInfo& info, const std::string& str, std::size_t& i, blet::Dict& dict) {
    dict.newArray();
    bool next = false;

    ++i; // jump '['
    s_stringJumpSpace(str, i);
    while (str[i] != ']' || (next == true && info.additionalNext == false)) {
        if (str[i] == '\0') {
            throw ParseException(info.filename, info.lastLine(i), info.lastColumn(i), "End of array not found");
        }
        // search array, object, string, number, bool or null
        if (!s_parseType(info, str, i, s_createNewArrayElement(dict))) {
            throw ParseException(info.filename, info.line(i), info.column(i), "Bad element of array");
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

static void s_parseObject(const JsonParseInfo& info, const std::string& str, std::size_t& i, blet::Dict& dict) {
    dict.newObject();
    bool next = false;

    ++i; // jump '{'
    s_stringJumpSpace(str, i);
    while (str[i] != '}' || (next == true && info.additionalNext == false)) {
        if (str[i] == '\0') {
            throw ParseException(info.filename, info.lastLine(i), info.lastColumn(i), "End of object not found");
        }
        else if (str[i] == '"') {
            // search array, object, string, number, bool or null
            if (!s_parseType(info, str, i, s_createNewObjectElement(info, str, i, dict))) {
                throw ParseException(info.filename, info.line(i), info.column(i), "Bad element in the key");
            }
        }
        else {
            throw ParseException(info.filename, info.line(i), info.column(i), "Key of object not found");
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

inline bool s_parseType(const JsonParseInfo& info, const std::string& str, std::size_t& i, blet::Dict& dict) {
    switch (str[i]) {
        case '[':
            s_parseArray(info, str, i, dict);
            break;
        case '{':
            s_parseObject(info, str, i, dict);
            break;
        case '"':
            s_parseString(info, str, i, dict);
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
            s_parseNumber(info, str, i, dict);
            break;
        case 't':
            if (!str.compare(i, sizeof("true") - 1, "true", sizeof("true") - 1)) {
                s_parseBool(true, i, dict);
            }
            else {
                return false;
            }
            break;
        case 'f':
            if (!str.compare(i, sizeof("false") - 1, "false", sizeof("false") - 1)) {
                s_parseBool(false, i, dict);
            }
            else {
                return false;
            }
            break;
        case 'n':
            if (!str.compare(i, sizeof("null") - 1, "null", sizeof("null") - 1)) {
                s_parseNull(i, dict);
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

static std::string s_streamToStr(JsonParseInfo& info, std::istream& stream) {
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

static blet::Dict s_parseStream(std::istream& stream, const std::string& filename, bool comment, bool additionalNext) {
    JsonParseInfo info(filename, additionalNext);
    std::string str = s_streamToStr(info, stream);
    if (comment) {
        s_replaceCommentBySpace(str);
    }
    std::size_t i = 0;
    s_stringJumpSpace(str, i);
    try {
        blet::Dict dict;
        switch (str[i]) {
            case '{':
                s_parseObject(info, str, i, dict);
                s_stringJumpSpace(str, i);
                break;
            case '[':
                s_parseArray(info, str, i, dict);
                s_stringJumpSpace(str, i);
                break;
            case '\0':
                return dict;
            default:
                throw ParseException(info.filename, info.line(i), info.column(i), "Not a valid start character");
        }
        if (str[i] != '\0') {
            throw ParseException(info.filename, info.line(i), info.column(i), "Not a valid end character");
        }
        return dict;
    }
    catch (const ParseException& /*e*/) {
        throw;
    }
}

blet::Dict parseFile(const char* filename, bool comment, bool additionalNext) {
    std::ifstream fileStream(filename); // open file
    if (!fileStream.is_open()) {
        throw ParseException(filename, "Open file failed");
    }
    return s_parseStream(fileStream, filename, comment, additionalNext);
}

blet::Dict parseStream(std::istream& stream, bool comment, bool additionalNext) {
    return s_parseStream(stream, std::string(), comment, additionalNext);
}

blet::Dict parseString(const std::string& str, bool comment, bool additionalNext) {
    std::istringstream iss(str);
    return parseStream(iss, comment, additionalNext);
}

blet::Dict parseData(const void* data, std::size_t size, bool comment, bool additionalNext) {
    return parseString(std::string(static_cast<const char*>(data), size), comment, additionalNext);
}

} // namespace json

} // namespace blet