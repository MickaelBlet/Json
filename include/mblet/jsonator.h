/**
 * jsonator.h
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

#ifndef _MBLET_JSONATOR_H_
#define _MBLET_JSONATOR_H_

#include <cstdio>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string>

namespace mblet {

/**
 * @brief Parse and create a json object
 */
class Jsonator : public std::map<std::string, Jsonator> {
  public:
    /**
     * @brief Parse exception from Exception
     */
    class ParseException : public std::exception {
      public:
        ParseException(const std::string& filename, const std::string& message) :
            std::exception(),
            _filename(filename),
            _message(message),
            _line(0),
            _column(0) {
            std::ostringstream oss("");
            oss << "Parse ";
            if (!filename.empty()) {
                oss << filename << ':';
            }
            oss << " (" << message << ").";
            _what = oss.str();
        };
        ParseException(const std::string& filename, std::size_t line, std::size_t column, const std::string& message) :
            std::exception(),
            _filename(filename),
            _message(message),
            _line(line),
            _column(column) {
            std::ostringstream oss("");
            oss << "Parse at ";
            if (!filename.empty()) {
                oss << filename << ':';
            }
            oss << line << ':' << column << " (" << message << ").";
            _what = oss.str();
        };
        virtual ~ParseException() throw() {}
        const char* what() const throw() {
            return _what.c_str();
        }
        const std::string& filename() const throw() {
            return _filename;
        }
        const std::string& message() const throw() {
            return _message;
        }
        std::size_t line() const throw() {
            return _line;
        }
        std::size_t column() const throw() {
            return _column;
        }

      protected:
        std::string _what;
        const std::string _filename;
        const std::string _message;
        const std::size_t _line;
        const std::size_t _column;
    };
    /**
     * @brief Access exception from std::exception
     */
    class AccessException : public std::exception {
      public:
        AccessException(const Jsonator& json, const char* message) :
            std::exception(),
            _message(message),
            _json(json) {
            std::ostringstream oss("");
            jsonToPath(oss, json);
            oss << ' ' << message << " (is " << getStrFromType(json._type) << ")" << '.';
            _what = oss.str();
        }
        virtual ~AccessException() throw() {}
        const char* what() const throw() {
            return _what.c_str();
        }
        const std::string& message() const throw() {
            return _message;
        }
        const Jsonator& json() const throw() {
            return _json;
        }

        static void jsonToPath(std::ostringstream& oss, const Jsonator& json) {
            std::stack<const Jsonator*> jsons;
            const Jsonator* pJson = json._parent;
            while (pJson != NULL) {
                jsons.push(pJson);
                pJson = pJson->_parent;
            }
            while (!jsons.empty()) {
                if (jsons.top()->_parent && jsons.top()->_parent->isObject()) {
                    oss << '\"' << jsons.top()->_key << '\"' << ':';
                }
                if (jsons.top()->_parent && jsons.top()->_parent->isArray()) {
                    oss << '[' << jsons.top()->_key << ']' << ':';
                }
                jsons.pop();
            }
            if (json._parent && json._parent->isObject()) {
                oss << '\"' << json._key << '\"';
            }
            if (json._parent && json._parent->isArray()) {
                oss << '[' << json._key << ']';
            }
        }

      protected:
        std::string _what;
        const std::string _message;
        const Jsonator& _json;
    };

    class ChildException : public AccessException {
      public:
        ChildException(const Jsonator& json, unsigned long index) :
            AccessException(json, "has not a child") {
            std::ostringstream oss("");
            jsonToPath(oss, json);
            oss << " out of range '" << index << "'.";
            _what = oss.str();
        }
        ChildException(const Jsonator& json, const std::string& child) :
            AccessException(json, "has not a child"),
            _child(child) {
            std::ostringstream oss("");
            jsonToPath(oss, json);
            oss << " has not a child '" << child << "'.";
            _what = oss.str();
        }
        virtual ~ChildException() throw() {}
        const std::string& child() const throw() {
            return _child;
        }

      protected:
        const std::string _child;
    };

    enum Type {
        NONE = 0,
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOLEAN
    };

    static const char* getStrFromType(const Type& type) {
        switch (type) {
            case NONE:
                return "NONE";
            case OBJECT:
                return "OBJECT";
            case ARRAY:
                return "ARRAY";
            case STRING:
                return "STRING";
            case NUMBER:
                return "NUMBER";
            case BOOLEAN:
                return "BOOLEAN";
            default:
                return "UNKNOWN";
        }
    }

    /**
     * @brief overide operator stream for print value
     *
     * @param os
     * @param json
     * @return std::ostream&
     */
    friend std::ostream& operator<<(std::ostream& os, const Jsonator& json) {
        os << json.dump();
        return os;
    }

    /**
     * @brief Construct the Jsonator object
     */
    Jsonator();

    /**
     * @brief Construct a new Jsonator object
     *
     * @param key
     */
    Jsonator(const Jsonator* parent, const std::string& key);

    /**
     * @brief Construct a new Jsonator object
     *
     * @param rhs
     */
    Jsonator(const Jsonator& rhs);

    /**
     * @brief Destroy the Jsonator object
     */
    virtual ~Jsonator();

    /**
     * @brief overide operator for copy json
     *
     * @param json
     */
    void operator=(const Jsonator& json) {
        _key = json._key;
        _string = json._string;
        _number = json._number;
        _boolean = json._boolean;
        _type = json._type;
        _filename = json._filename;
        std::map<std::string, Jsonator>::operator=(json);
    }

    /**
     * @brief overide operator for set value from str
     *
     * @param value : new value
     */
    void operator=(const std::string& value) {
        newString(value);
    }

    /**
     * @brief overide operator for set value from str
     *
     * @param value : new value
     */
    void operator=(const char* value) {
        newString(value);
    }

    /**
     * @brief overide operator for set value from str
     *
     * @param value : new value
     */
    template<size_t Size>
    void operator=(const char (&value)[Size]) {
        newString(std::string(value, Size - 1));
    }

    /**
     * @brief overide operator for set value from bool
     *
     * @param value : new value
     */
    void operator=(const bool& value) {
        newBoolean(value);
    }

    /**
     * @brief overide operator for set value from number
     *
     * @tparam T
     * @param value : new value
     */
    template<typename T>
    void operator=(const T& value) {
        newNumber(value);
    }

    iterator find(const std::string& str) {
        return std::map<std::string, Jsonator>::find(str);
    }

    iterator find(const char* str) {
        return std::map<std::string, Jsonator>::find(str);
    }

    template<size_t Size>
    iterator find(const char (&str)[Size]) {
        return std::map<std::string, Jsonator>::find(std::string(str, Size - 1));
    }

    template<typename T>
    iterator find(const T& index) {
        if (index < 0) {
            return std::map<std::string, Jsonator>::find(indexToString(size() + index));
        }
        else {
            return std::map<std::string, Jsonator>::find(indexToString(index));
        }
    }

    const_iterator find(const std::string& str) const {
        return std::map<std::string, Jsonator>::find(str);
    }

    const_iterator find(const char* str) const {
        return std::map<std::string, Jsonator>::find(str);
    }

    template<size_t Size>
    const_iterator find(const char (&str)[Size]) const {
        return std::map<std::string, Jsonator>::find(std::string(str, Size - 1));
    }

    template<typename T>
    const_iterator find(const T& index) const {
        if (index < 0) {
            return std::map<std::string, Jsonator>::find(indexToString(size() + index));
        }
        else {
            return std::map<std::string, Jsonator>::find(indexToString(index));
        }
    }

    /**
     * @brief overide operator for get json from string
     *
     * @param str : at string
     * @return Jsonator& : json from string
     */
    Jsonator& operator[](const std::string& str) {
        if (_type == NONE) {
            _type = OBJECT;
        }
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        iterator it = find(str);
        if (it != end()) {
            return it->second;
        }
        return insert(std::pair<std::string, Jsonator>(str, Jsonator(this, str))).first->second;
    }

    /**
     * @brief overide operator for get const json from string
     *
     * @param str : search string
     * @return const Jsonator& : json from string
     */
    const Jsonator& operator[](const std::string& str) const {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        const_iterator it = find(str);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, str);
    }

    Jsonator& operator[](const char* str) {
        return operator[](std::string(str));
    }

    const Jsonator& operator[](const char* str) const {
        return operator[](std::string(str));
    }

    template<size_t Size>
    Jsonator& operator[](const char (&str)[Size]) {
        return operator[](std::string(str, Size - 1));
    }

    template<size_t Size>
    const Jsonator& operator[](const char (&str)[Size]) const {
        return operator[](std::string(str, Size - 1));
    }

    template<typename T>
    Jsonator& operator[](const T& index) {
        if (_type == NONE) {
            _type = ARRAY;
        }
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        std::string str;
        if (index < 0) {
            str = indexToString(size() + index);
        }
        else {
            str = indexToString(index);
        }
        iterator it = find(str);
        if (it != end()) {
            return it->second;
        }
        while (static_cast<unsigned long>(size()) < static_cast<unsigned long>(index)) {
            std::string strTmp = indexToString(size());
            insert(std::pair<std::string, Jsonator>(strTmp, Jsonator(this, strTmp)));
        }
        return insert(std::pair<std::string, Jsonator>(str, Jsonator(this, str))).first->second;
    }

    template<typename T>
    const Jsonator& operator[](const T& index) const {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        const_iterator it = find(index);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, index);
    }

    Jsonator& at(const std::string& str) {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        iterator it = find(str);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, str);
    }

    const Jsonator& at(const std::string& str) const {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        const_iterator it = find(str);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, str);
    }

    Jsonator& at(const char* str) {
        return at(std::string(str));
    }

    const Jsonator& at(const char* str) const {
        return at(std::string(str));
    }

    template<size_t Size>
    Jsonator& at(const char (&str)[Size]) {
        return at(std::string(str, Size - 1));
    }

    template<size_t Size>
    const Jsonator& at(const char (&str)[Size]) const {
        return at(std::string(str, Size - 1));
    }

    template<typename T>
    Jsonator& at(const T& index) {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        iterator it = find(index);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, index);
    }

    template<typename T>
    const Jsonator& at(const T& index) const {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        const_iterator it = find(index);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, index);
    }

    template<typename T>
    void push_front(const T& value) {
        for (std::size_t i = size(); i > 0; --i) {
            Jsonator& m = at(i - 1);
            m._key = indexToString(i);
            operator[](i) = m;
        }
        {
            // create and replace first value
            Jsonator m(this, "0");
            m = value;
            operator[](0) = m;
        }
    }

    template<typename T>
    void push_back(const T& value) {
        operator[](size()) = value;
    }

    void pop_front() {
        erase(0);
    }

    void pop_back() {
        erase(size() - 1);
    }

    /**
     * @brief create object
     */
    void newObject() {
        if (_type != NONE && _type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        _type = OBJECT;
    }

    /**
     * @brief create array
     */
    void newArray() {
        if (_type != NONE && _type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        _type = ARRAY;
    }

    /**
     * @brief create string from template
     *
     * @tparam T : type of value
     * @param value
     */
    template<typename T>
    void newString(const T& value) {
        if (_type != NONE && _type != STRING) {
            throw AccessException(*this, "is not a string");
        }
        std::ostringstream oss("");
        oss << value;
        _string = oss.str();
        _type = STRING;
    }

    /**
     * @brief create number from template
     *
     * @tparam T : type of value
     * @param value
     */
    template<typename T>
    void newNumber(const T& value) {
        if (_type != NONE && _type != NUMBER) {
            throw AccessException(*this, "is not a number");
        }
        _number = value;
        _type = NUMBER;
    }

    /**
     * @brief create bool from template
     *
     * @tparam T : type of value
     * @param value
     */
    template<typename T>
    void newBoolean(const T& value) {
        if (_type != NONE && _type != BOOLEAN) {
            throw AccessException(*this, "is not a boolean");
        }
        _boolean = value;
        _type = BOOLEAN;
    }

    /**
     * @brief create null
     */
    void newNull() {
        if (_type != NONE) {
            throw AccessException(*this, "is not a null");
        }
        _type = NONE;
    }

    bool isNull() const {
        return _type == NONE;
    }

    bool isObject() const {
        return _type == OBJECT;
    }

    bool isArray() const {
        return _type == ARRAY;
    }

    bool isString() const {
        return _type == STRING;
    }

    bool isNumber() const {
        return _type == NUMBER;
    }

    bool isBoolean() const {
        return _type == BOOLEAN;
    }

    std::string dump(std::size_t indent = 0) const;

    Jsonator& erase(const std::string& str) {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        iterator it = find(str);
        if (it != end()) {
            std::map<std::string, Jsonator>::erase(str);
            if (empty()) {
                _type = NONE;
            }
        }
        else {
            throw ChildException(*this, str);
        }
        return *this;
    }

    Jsonator& erase(const char* str) {
        return erase(std::string(str));
    }

    template<size_t Size>
    Jsonator& erase(const char (&str)[Size]) {
        return erase(std::string(str, Size - 1));
    }

    template<typename T>
    Jsonator& erase(const T& index) {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        iterator it = find(index);
        if (it != end()) {
            for (std::size_t i = index; i < size() - 1; ++i) {
                Jsonator& m = at(i + 1);
                m._key = indexToString(i);
                operator[](i) = m;
            }
            std::map<std::string, Jsonator>::erase(indexToString(size() - 1));
            if (empty()) {
                _type = NONE;
            }
        }
        else {
            throw ChildException(*this, index);
        }
        return *this;
    }

    const Jsonator* getParent() const {
        return _parent;
    }

    const std::string& getKey() const {
        return _key;
    }

    const std::string& getString() const {
        if (_type != STRING) {
            throw AccessException(*this, "is not a string");
        }
        return _string;
    }

    const double& getNumber() const {
        if (_type != NUMBER) {
            throw AccessException(*this, "is not a number");
        }
        return _number;
    }

    const bool& getBoolean() const {
        if (_type != BOOLEAN) {
            throw AccessException(*this, "is not a boolean");
        }
        return _boolean;
    }

    const Type& getType() const {
        return _type;
    }

    template<typename T>
    T get() const {
        T ret;
        std::stringstream ss("");
        switch (_type) {
            case NONE:
                ss << static_cast<void*>(0);
                break;
            case OBJECT:
            case ARRAY:
                ss << *this;
                break;
            case STRING:
                ss << _string;
                break;
            case NUMBER:
                ss << _number;
                break;
            case BOOLEAN:
                ss << _boolean;
                break;
        }
        if (ss >> ret) {
            return ret;
        }
        else {
            throw AccessException(*this, "bad convertion to other type");
        }
    }

    template<typename T>
    void get(T& ret) const {
        ret = get<T>();
    }

    operator const std::string&() const {
        return getString();
    }

    operator const char*() const {
        return getString().c_str();
    }

    operator const bool&() const {
        return getBoolean();
    }

    template<typename T>
    operator T() const {
        return getNumber();
    }

    /**
     * @brief read file and load its config
     *
     * @param filename
     * @return true
     * @return false
     */
    void parseFile(const char* filename, bool comment = true, bool additionalNext = true);

    /**
     * @brief read stream and load its config
     *
     * @param stream
     */
    void parseStream(std::istream& stream, bool comment = true, bool additionalNext = true) {
        _parseStream(stream, comment, additionalNext);
    }

    /**
     * @brief read string and load its config
     *
     * @param str
     */
    void parseString(const std::string& str, bool comment = true, bool additionalNext = true) {
        std::istringstream iss(str);
        parseStream(iss, comment, additionalNext);
    }

    /**
     * @brief read data and load its config
     *
     * @param data
     * @param size
     */
    void parseData(const void* data, std::size_t size, bool comment = true, bool additionalNext = true) {
        parseString(std::string(static_cast<const char*>(data), size), comment, additionalNext);
    }

    const std::string& getFilename() const {
        return _filename;
    }

    const Jsonator& getConst() const {
        return *this;
    }

  private:
    static std::string indexToString(unsigned long index) {
        char str[32];
        ::snprintf(str, sizeof(str), "%lu", index);
        return str;
    }
    void _parseStream(std::istream& stream, bool comment, bool additionalNext);

    const Jsonator* _parent;
    std::string _key;
    std::string _string;
    double _number;
    bool _boolean;
    Type _type;
    std::string _filename;
};

} // namespace mblet

#endif // _MBLET_JSONATOR_H_