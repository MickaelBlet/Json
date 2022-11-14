/**
 * jsonatorJson.cpp
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

#include <iomanip>
#include <limits>

#include "mblet/jsonator.h"

namespace mblet {

Jsonator::Json::Json() :
    std::map<std::string, Json>(),
    _parent(NULL),
    _key(""),
    _string("null"),
    _number(0),
    _boolean(false),
    _type(NONE) {}

Jsonator::Json::Json(const Json* parent, const std::string& key) :
    std::map<std::string, Json>(),
    _parent(parent),
    _key(key),
    _string("null"),
    _number(0),
    _boolean(false),
    _type(NONE) {}

Jsonator::Json::Json(const Json& rhs) :
    std::map<std::string, Json>(rhs),
    _parent(rhs._parent),
    _key(rhs._key),
    _string(rhs._string),
    _number(rhs._number),
    _boolean(rhs._boolean),
    _type(rhs._type) {}

Jsonator::Json::~Json() {}

static inline void s_newlineDump(std::ostream& oss, const Jsonator::Json& json, std::size_t indent) {
    if (!json.empty() && indent != 0) {
        oss << '\n';
    }
}

static inline void s_indentDump(std::ostream& oss, const Jsonator::Json& json, std::size_t indent, std::size_t index) {
    if (!json.empty() && indent != 0) {
        oss << std::string(indent * index, ' ');
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

static void s_typeDump(std::ostream& oss, const Jsonator::Json& json, std::size_t indent, std::size_t index = 0);

static void s_objectDump(std::ostream& oss, const Jsonator::Json& json, std::size_t indent, std::size_t index) {
    oss << '{';
    s_newlineDump(oss, json, indent);
    ++index;
    for (Jsonator::Json::const_iterator cit = json.begin(); cit != json.end(); ++cit) {
        if (cit != json.begin()) {
            oss << ',';
            s_newlineDump(oss, json, indent);
        }
        s_indentDump(oss, json, indent, index);
        s_stringDump(oss, cit->second.getKey()); // key
        oss << ((indent != 0) ? ": " : ":");
        s_typeDump(oss, cit->second, indent, index);
    }
    s_newlineDump(oss, json, indent);
    --index;
    s_indentDump(oss, json, indent, index);
    oss << '}';
}

static void s_arrayDump(std::ostream& oss, const Jsonator::Json& json, std::size_t indent, std::size_t index) {
    oss << '[';
    s_newlineDump(oss, json, indent);
    ++index;
    for (std::size_t i = 0; i < json.size(); ++i) {
        if (i != 0) {
            oss << ',';
            s_newlineDump(oss, json, indent);
        }
        s_indentDump(oss, json, indent, index);
        s_typeDump(oss, json[i], indent, index);
    }
    s_newlineDump(oss, json, indent);
    --index;
    s_indentDump(oss, json, indent, index);
    oss << ']';
}

void s_typeDump(std::ostream& oss, const Jsonator::Json& json, std::size_t indent, std::size_t index) {
    switch (json.getType()) {
        case Jsonator::Json::ARRAY:
            s_arrayDump(oss, json, indent, index);
            break;
        case Jsonator::Json::OBJECT:
            s_objectDump(oss, json, indent, index);
            break;
        case Jsonator::Json::STRING:
            s_stringDump(oss, json.getString());
            break;
        case Jsonator::Json::NUMBER:
            oss << json.getNumber();
            break;
        case Jsonator::Json::BOOLEAN:
            oss << ((json.getBoolean()) ? "true" : "false");
            break;
        case Jsonator::Json::NONE:
            oss << "null";
            break;
    }
}

std::string Jsonator::Json::dump(std::size_t indent) const {
    std::ostringstream oss("");
    oss << std::setprecision(std::numeric_limits<double>::digits10 + 1);
    s_typeDump(oss, *this, indent);
    return oss.str();
}

} // namespace mblet