/**
 * dump.cpp
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2024 BLET Mickaël.
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

#include <iomanip> // std::setprecision
#include <limits>  // std::numeric_limits

#include "blet/json.h"

namespace blet {

namespace json {

class Dumper {
  public:
    Dumper(std::ostream& os, std::size_t indent, char indentCharacter) :
        os_(os),
        indent_(indent),
        indentCharacter_(indentCharacter) {}

    ~Dumper() {}

    void typeDump(const blet::Dict& dict, std::size_t index = 0) {
        switch (dict.getType()) {
            case blet::Dict::NULL_TYPE:
                os_.write("null", sizeof("null") - 1);
                break;
            case blet::Dict::OBJECT_TYPE:
                objectDump(dict, index);
                break;
            case blet::Dict::ARRAY_TYPE:
                arrayDump(dict, index);
                break;
            case blet::Dict::STRING_TYPE:
                os_ << '"';
                stringEscape(dict.getValue().getString());
                os_ << '"';
                break;
            case blet::Dict::NUMBER_TYPE:
                os_ << dict.getValue().getNumber();
                break;
            case blet::Dict::BOOLEAN_TYPE:
                if (dict.getValue().getBoolean()) {
                    os_.write("true", sizeof("true") - 1);
                }
                else {
                    os_.write("false", sizeof("false") - 1);
                }
                break;
        }
    }

    void objectDump(const blet::Dict& dict, std::size_t index) {
        os_ << '{';
        newlineDump(dict);
        ++index;
        for (blet::Dict::object_t::const_iterator cit = dict.getValue().getObject().begin();
             cit != dict.getValue().getObject().end(); ++cit) {
            if (cit != dict.getValue().getObject().begin()) {
                os_ << ',';
                newlineDump(dict);
            }
            indentDump(dict, index);
            os_ << '"';
            stringEscape(cit->first);
            os_ << '"';
            os_ << ':';
            if (indent_ != 0) {
                os_ << ' ';
            }
            typeDump(cit->second, index);
        }
        newlineDump(dict);
        --index;
        indentDump(dict, index);
        os_ << '}';
    }

    void arrayDump(const blet::Dict& dict, std::size_t index) {
        os_ << '[';
        newlineDump(dict);
        ++index;
        for (std::size_t i = 0; i < dict.getValue().getArray().size(); ++i) {
            if (i != 0) {
                os_ << ',';
                newlineDump(dict);
            }
            indentDump(dict, index);
            typeDump(dict.getValue().getArray()[i], index);
        }
        newlineDump(dict);
        --index;
        indentDump(dict, index);
        os_ << ']';
    }

    void stringEscape(const std::string& str) {
        for (std::size_t i = 0; i < str.size(); ++i) {
            switch (str[i]) {
                case '\a':
                    os_ << '\\' << 'a';
                    break;
                case '\b':
                    os_ << '\\' << 'b';
                    break;
                case '\f':
                    os_ << '\\' << 'f';
                    break;
                case '\n':
                    os_ << '\\' << 'n';
                    break;
                case '\r':
                    os_ << '\\' << 'r';
                    break;
                case '\t':
                    os_ << '\\' << 't';
                    break;
                case '\v':
                    os_ << '\\' << 'v';
                    break;
                case '\'':
                    os_ << '\'';
                    break;
                case '"':
                    os_ << '\\' << '"';
                    break;
                case '\\':
                    os_ << '\\' << '\\';
                    break;
                default:
                    os_ << str[i];
                    break;
            }
        }
    }

    void newlineDump(const blet::Dict& dict) {
        if (indent_ != 0) {
            if ((dict.getType() == blet::Dict::OBJECT_TYPE && !dict.getValue().getObject().empty()) ||
                (dict.getType() == blet::Dict::ARRAY_TYPE && !dict.getValue().getArray().empty())) {
                os_ << '\n';
            }
        }
    }

    void indentDump(const blet::Dict& dict, std::size_t index) {
        if (indent_ != 0) {
            if ((dict.getType() == blet::Dict::OBJECT_TYPE && !dict.getValue().getObject().empty()) ||
                (dict.getType() == blet::Dict::ARRAY_TYPE && !dict.getValue().getArray().empty())) {
                os_ << std::string(indent_ * index, indentCharacter_);
            }
        }
    }

  private:
    std::ostream& os_;
    std::size_t indent_;
    char indentCharacter_;
};

void dump(const blet::Dict& dict, std::ostream& os, std::size_t indent, char indentCharacter) {
    os << std::setprecision(std::numeric_limits<double>::digits10 + 1);
    Dumper dumper(os, indent, indentCharacter);
    dumper.typeDump(dict);
}

std::string dump(const blet::Dict& dict, std::size_t indent, char indentCharacter) {
    std::ostringstream os("");
    dump(dict, os, indent, indentCharacter);
    return os.str();
}

} // namespace json

} // namespace blet
