/**
 * load.cpp
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

#include <fstream> // std::ifstream

#include "blet/json.h"

namespace blet {

namespace json {

class StringReader {
  public:
    StringReader(std::istream& stream) :
        stream_(stream),
        pos_(0),
        line_(1),
        posColumn_(0) {
        stream_.seekg(0, stream_.end);
        std::size_t streamLength = stream_.tellg();
        stream_.seekg(0, stream_.beg);
        std::vector<char> vBuffer(streamLength);
        stream_.read(vBuffer.data(), vBuffer.size());
        str_.append(vBuffer.begin(), vBuffer.end());
    }

    ~StringReader() {}

    template<std::size_t Size>
    bool operator==(const char (&str)[Size]) {
        return !str_.compare(pos_, Size - 1, str, Size - 1);
    }

    void operator++() {
        if (str_[pos_] == '\n') {
            posColumn_ = pos_;
            ++line_;
        }
        ++pos_;
    }

    void operator+=(std::size_t index) {
        for (std::size_t i = 0; i < index; ++i) {
            operator++();
        }
    }

    const char& operator[](std::size_t index) {
        return str_[pos_ + index];
    }

    std::string substr(std::size_t start, std::size_t end) {
        return str_.substr(start, end - start);
    }

    std::size_t line() {
        return line_;
    }

    std::size_t column() {
        return pos_ - posColumn_ + 1;
    }

    std::size_t index() {
        return pos_;
    }

    std::istream& streamOffset(std::size_t offset) {
        return stream_.seekg(offset, stream_.beg);
    }

  private:
    std::istream& stream_;
    std::string str_;
    std::size_t pos_;
    std::size_t line_;
    std::size_t posColumn_;
};

class Loader {
  public:
    Loader(const std::string& filename, bool comment, bool additionnalNext, std::istream& stream) :
        filename_(filename),
        comment_(comment),
        additionalNext_(additionnalNext),
        reader_(stream) {}

    ~Loader() {}

    void load(blet::Dict& dict) {
        spaceJump();
        switch (reader_[0]) {
            case '{':
                loadObject(dict);
                spaceJump();
                break;
            case '[':
                loadArray(dict);
                spaceJump();
                break;
            case '\0':
                break;
            default:
                throw LoadException(filename_, reader_.line(), reader_.column(), "Not a valid start character");
        }
        if (reader_[0] != '\0') {
            throw LoadException(filename_, reader_.line(), reader_.column(), "Not a valid end character");
        }
    }

    void loadObject(blet::Dict& dict) {
        dict.newObject();
        bool next = false;
        ++reader_; // jump '{'
        spaceJump();
        while (reader_[0] != '}' || (next == true && additionalNext_ == false)) {
            if (reader_[0] == '\0') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "End of object not found");
            }
            else if (reader_[0] == '"') {
                // search array, object, string, number, bool or null
                if (!loadType(createNewObjectElement(dict))) {
                    throw LoadException(filename_, reader_.line(), reader_.column(), "Bad element in the key");
                }
            }
            else {
                throw LoadException(filename_, reader_.line(), reader_.column(), "Key of object not found");
            }
            spaceJump();
            // next
            if (reader_[0] == ',') {
                ++reader_; // jump ','
                next = true;
                spaceJump();
            }
            else {
                next = false;
            }
        }
        ++reader_; // jump '}'
    }

    void loadArray(blet::Dict& dict) {
        dict.newArray();
        bool next = false;

        ++reader_; // jump '['
        spaceJump();
        while (reader_[0] != ']' || (next == true && additionalNext_ == false)) {
            if (reader_[0] == '\0') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "End of array not found");
            }
            // search array, object, string, number, bool or null
            if (!loadType(createNewArrayElement(dict))) {
                throw LoadException(filename_, reader_.line(), reader_.column(), "Bad element of array");
            }
            spaceJump();
            // next
            if (reader_[0] == ',') {
                ++reader_; // jump ','
                next = true;
                spaceJump();
            }
            else {
                next = false;
            }
        }
        ++reader_; // jump ']'
    }

    blet::Dict& createNewObjectElement(blet::Dict& dict) {
        // parser key
        ++reader_; // jump '"'
        std::size_t start = reader_.index();
        // search end quote
        while (reader_[0] != '"') {
            if (reader_[0] == '\\' && (reader_[1] == '"' || reader_[1] == '\\')) {
                ++reader_;
            }
            else if (reader_[0] == '\0') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "End of key");
            }
            else if (reader_[0] == '\n') {
                throw LoadException(filename_, reader_.line(), start + 1, "New line in key");
            }
            ++reader_;
        }
        // get key
        std::string key = stringEscape(reader_.substr(start, reader_.index()));
        if (dict.getValue().getObject().find(key) != dict.getValue().getObject().end()) {
            throw LoadException(filename_, reader_.line(), start + 1, "Key already exist");
        }
        ++reader_; // jump '"'
        spaceJump();
        if (reader_[0] != ':') {
            throw LoadException(filename_, reader_.line(), reader_.column(), "Need definition of object");
        }
        ++reader_; // jump ':'
        spaceJump();
        return dict.getValue().getObject()[key];
    }

    blet::Dict& createNewArrayElement(blet::Dict& dict) {
        dict.getValue().getArray().push_back(blet::Dict());
        return dict.getValue().getArray().back();
    }

    bool loadType(blet::Dict& dict) {
        switch (reader_[0]) {
            case '[':
                loadArray(dict);
                break;
            case '{':
                loadObject(dict);
                break;
            case '"':
                this->loadString(dict);
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
                loadNumber(dict);
                break;
            case 't':
                if (reader_ == "true") {
                    loadBool(true, dict);
                }
                else {
                    return false;
                }
                break;
            case 'f':
                if (reader_ == "false") {
                    loadBool(false, dict);
                }
                else {
                    return false;
                }
                break;
            case 'n':
                if (reader_ == "null") {
                    loadNull(dict);
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

    void loadNull(blet::Dict& dict) {
        dict.newNull();
        reader_ += (sizeof("null") - 1);
    }

    void loadBool(bool boolean, blet::Dict& dict) {
        dict.newBoolean(boolean);
        if (boolean) {
            reader_ += (sizeof("true") - 1);
        }
        else {
            reader_ += (sizeof("false") - 1);
        }
    }

    void loadNumber(blet::Dict& dict) {
        if (reader_[0] == '0' && reader_[1] >= '0' && reader_[1] <= '9') {
            throw LoadException(filename_, reader_.line(), reader_.column(), "Octal number not allowed");
        }
        double number;
        std::istream& ris = reader_.streamOffset(reader_.index());
        if (ris >> number) {
            dict.newNumber(number);
            // jump of string number size
            reader_ += (ris.tellg() - std::streampos(reader_.index()));
        }
        else {
            throw LoadException(filename_, reader_.line(), reader_.column(), "Bad number");
        }
    }

    void loadString(blet::Dict& dict) {
        ++reader_; // jump '"'
        std::size_t start = reader_.index();
        // search end quote
        while (reader_[0] != '"') {
            if (reader_[0] == '\\' && (reader_[1] == '"' || reader_[1] == '\\')) {
                ++reader_;
            }
            if (reader_[0] == '\0') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "End of string");
            }
            if (reader_[0] == '\n') {
                throw LoadException(filename_, reader_.line(), start + 1, "New line in string");
            }
            ++reader_;
        }
        dict.newString(stringEscape(reader_.substr(start, reader_.index())));
        ++reader_; // jump '"'
    }

    static std::string stringEscape(const std::string& str) {
        std::ostringstream oss("");
        for (std::size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '\\') {
                switch (str[i + 1]) {
                    case 'a':
                        oss << '\a';
                        break;
                    case 'b':
                        oss << '\b';
                        break;
                    case 'f':
                        oss << '\f';
                        break;
                    case 'n':
                        oss << '\n';
                        break;
                    case 'r':
                        oss << '\r';
                        break;
                    case 't':
                        oss << '\t';
                        break;
                    case 'v':
                        oss << '\v';
                        break;
                    case '\'':
                        oss << '\'';
                        break;
                    case '"':
                        oss << '\"';
                        break;
                    case '\\':
                        oss << '\\';
                        break;
                }
                ++i;
            }
            else {
                oss << str[i];
            }
        }
        return oss.str();
    }

    void spaceJump() {
        commentJump();
        // isspace
        while ((reader_[0] >= '\t' && reader_[0] <= '\r') || reader_[0] == ' ') {
            ++reader_;
            commentJump();
        }
    }

    void commentJump() {
        if (comment_) {
            if (reader_[0] == '/' && reader_[1] == '*') {
                reader_ += (sizeof("/*") - 1); // jump "/*"
                while (reader_[0] != '\0' && (reader_[0] != '*' || reader_[1] != '/')) {
                    ++reader_; // jump character
                }
                if (reader_[0] != '\0') {
                    reader_ += (sizeof("*/") - 1); // jump "*/"
                }
            }
            else if (reader_[0] == '/' && reader_[1] == '/') {
                while (reader_[0] != '\0' && reader_[0] != '\n') {
                    ++reader_; // jump character
                }
                if (reader_[0] != '\0') {
                    ++reader_; // jump '\n'
                }
            }
        }
    }

  private:
    const std::string filename_;
    bool comment_;
    bool additionalNext_;
    StringReader reader_;
};

blet::Dict loadFile(const char* filename, bool comment, bool additionalNext) {
    std::ifstream fileStream(filename); // open file
    if (!fileStream.is_open()) {
        throw LoadException(filename, "Open file failed");
    }
    blet::Dict dict;
    Loader loader(filename, comment, additionalNext, fileStream);
    loader.load(dict);
    return dict;
}

blet::Dict loadStream(std::istream& stream, bool comment, bool additionalNext) {
    blet::Dict dict;
    Loader loader("", comment, additionalNext, stream);
    loader.load(dict);
    return dict;
}

blet::Dict loadString(const std::string& str, bool comment, bool additionalNext) {
    std::istringstream iss(str);
    return loadStream(iss, comment, additionalNext);
}

blet::Dict loadData(const void* data, std::size_t size, bool comment, bool additionalNext) {
    return loadString(std::string(static_cast<const char*>(data), size), comment, additionalNext);
}

} // namespace json

} // namespace blet
