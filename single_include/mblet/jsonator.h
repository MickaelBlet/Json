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

#include <deque>     // std::deque
#include <exception> // std::exception
#include <list>      // std::list
#include <map>       // std::map
#include <queue>     // std::queue
#include <set>       // std::set
#include <sstream>   // std::istream, std::ostream, std:ostringstream
#include <stack>     // std::stack
#include <string>    // std::string
#include <vector>    // std::vector

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
        const std::size_t& line() const throw() {
            return _line;
        }
        const std::size_t& column() const throw() {
            return _column;
        }

      protected:
        std::string _what;
        std::string _filename;
        std::string _message;
        std::size_t _line;
        std::size_t _column;
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
            if (json._parent) {
                jsonToPath(oss, json);
                oss << ' ';
            }
            oss << message << " (is " << getStrFromType(json._type) << ")" << '.';
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
        std::string _message;
        const Jsonator& _json;
    };

    /**
     * @brief Child exception from AccessException
     */
    class ChildException : public AccessException {
      public:
        ChildException(const Jsonator& json, unsigned long index) :
            AccessException(json, "out of range"),
            _index(index) {
            std::ostringstream oss("");
            if (json._parent) {
                jsonToPath(oss, json);
                oss << ' ';
            }
            oss << _message << " '" << index << "'.";
            _what = oss.str();
        }
        ChildException(const Jsonator& json, const std::string& child) :
            AccessException(json, "has not a child"),
            _child(child) {
            std::ostringstream oss("");
            if (json._parent) {
                jsonToPath(oss, json);
                oss << ' ';
            }
            oss << _message << " '" << child << "'.";
            _what = oss.str();
        }
        virtual ~ChildException() throw() {}
        const std::string& child() const throw() {
            return _child;
        }
        const unsigned long& index() const throw() {
            return _index;
        }

      protected:
        unsigned long _index;
        std::string _child;
    };

    /**
     * @brief type of jsonator
     */
    enum Type {
        NONE = 0,
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOLEAN
    };

    /**
     * @brief Get the Str From Type
     *
     * @param type
     * @return const char*
     */
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
     * @brief override operator stream for print value
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
     * @param parent
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
    ~Jsonator();

    /**
     * @brief copy json
     *
     * @param json
     */
    void operator=(const Jsonator& json);

    /**
     * @brief set value from str
     *
     * @param value : new value
     */
    void operator=(const std::string& value) {
        newString(value);
    }

    /**
     * @brief set value from str
     *
     * @param value : new value
     */
    void operator=(const char* value) {
        newString(value);
    }

    /**
     * @brief set value from str
     *
     * @param value : new value
     */
    template<std::size_t Size>
    void operator=(const char (&value)[Size]) {
        newString(std::string(value));
    }

    /**
     * @brief set value from bool
     *
     * @param value : new value
     */
    void operator=(const bool& value) {
        newBoolean(value);
    }

    /**
     * @brief set value from deque
     *
     * @tparam T
     * @param value
     */
    template<typename T>
    void operator=(const std::deque<T>& value) {
        typename std::deque<T>::const_iterator it;
        for (it = value.begin(); it != value.end(); ++it) {
            push_back(*it);
        }
    }

    /**
     * @brief set value from list
     *
     * @tparam T
     * @param value
     */
    template<typename T>
    void operator=(const std::list<T>& value) {
        typename std::list<T>::const_iterator it;
        for (it = value.begin(); it != value.end(); ++it) {
            push_back(*it);
        }
    }

    /**
     * @brief set value from map
     *
     * @tparam T key
     * @tparam U value
     * @param value
     */
    template<typename T, typename U>
    void operator=(const std::map<T, U>& value) {
        typename std::map<T, U>::const_iterator it;
        for (it = value.begin(); it != value.end(); ++it) {
            operator[](it->first) = it->second;
        }
    }

    /**
     * @brief set value from queue
     *
     * @tparam T
     * @param value
     */
    template<typename T>
    void operator=(const std::queue<T>& value) {
        std::queue<T> copy = value;
        while (!copy.empty()) {
            push_back(copy.front());
            copy.pop();
        }
    }

    /**
     * @brief set value from set
     *
     * @tparam T
     * @param value
     */
    template<typename T>
    void operator=(const std::set<T>& value) {
        typename std::set<T>::const_iterator it;
        for (it = value.begin(); it != value.end(); ++it) {
            push_back(*it);
        }
    }

    /**
     * @brief set value from stack
     *
     * @tparam T
     * @param value
     */
    template<typename T>
    void operator=(const std::stack<T>& value) {
        std::stack<T> copy = value;
        while (!copy.empty()) {
            push_back(copy.top());
            copy.pop();
        }
    }

    /**
     * @brief set value from vector
     *
     * @tparam T
     * @param value
     */
    template<typename T>
    void operator=(const std::vector<T>& value) {
        typename std::vector<T>::const_iterator it;
        for (it = value.begin(); it != value.end(); ++it) {
            push_back(*it);
        }
    }

    /**
     * @brief set value from number
     *
     * @tparam T
     * @param value
     */
    template<typename T>
    void operator=(const T& value) {
        newNumber(value);
    }

    /**
     * @brief find by key
     *
     * @param key
     * @return iterator
     */
    iterator find(const std::string& key) {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        return std::map<std::string, Jsonator>::find(key);
    }

    /**
     * @brief find by key
     *
     * @param key
     * @return const_iterator
     */
    const_iterator find(const std::string& key) const {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        return std::map<std::string, Jsonator>::find(key);
    }

    /**
     * @brief find by key
     *
     * @param key
     * @return iterator
     */
    iterator find(const char* key) {
        return find(std::string(key));
    }

    /**
     * @brief find by key
     *
     * @param key
     * @return const_iterator
     */
    const_iterator find(const char* key) const {
        return find(std::string(key));
    }

    /**
     * @brief find by key
     *
     * @tparam Size
     * @return iterator
     */
    template<std::size_t Size>
    iterator find(const char (&key)[Size]) {
        return find(std::string(key));
    }

    /**
     * @brief find by key
     *
     * @tparam Size
     * @return const_iterator
     */
    template<std::size_t Size>
    const_iterator find(const char (&key)[Size]) const {
        return find(std::string(key));
    }

    /**
     * @brief find by index
     *
     * @tparam T
     * @param index
     * @return iterator
     */
    template<typename T>
    iterator find(const T& index) {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        return std::map<std::string, Jsonator>::find(indexToString(index));
    }

    /**
     * @brief find by index
     *
     * @tparam T
     * @param index
     * @return const_iterator
     */
    template<typename T>
    const_iterator find(const T& index) const {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        return std::map<std::string, Jsonator>::find(indexToString(index));
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator&
     */
    Jsonator& operator[](const std::string& key) {
        if (_type == NONE) {
            _type = OBJECT;
        }
        iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        return std::map<std::string, Jsonator>::insert(std::pair<std::string, Jsonator>(key, Jsonator(this, key)))
            .first->second;
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     */
    const Jsonator& operator[](const std::string& key) const {
        const_iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, key);
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator&
     */
    Jsonator& operator[](const char* key) {
        return operator[](std::string(key));
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     */
    const Jsonator& operator[](const char* key) const {
        return operator[](std::string(key));
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator&
     */
    template<std::size_t Size>
    Jsonator& operator[](const char (&key)[Size]) {
        return operator[](std::string(key));
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     */
    template<std::size_t Size>
    const Jsonator& operator[](const char (&key)[Size]) const {
        return operator[](std::string(key));
    }

    /**
     * @brief get json from @p index
     *
     * @param index
     * @return Jsonator&
     */
    template<typename T>
    Jsonator& operator[](const T& index) {
        if (_type == NONE) {
            _type = ARRAY;
        }
        iterator it = find(index);
        if (it != end()) {
            return it->second;
        }
        while (static_cast<unsigned long>(size()) < static_cast<unsigned long>(index)) {
            std::string strTmp = indexToString(size());
            std::map<std::string, Jsonator>::insert(std::pair<std::string, Jsonator>(strTmp, Jsonator(this, strTmp)));
        }
        std::string str(indexToString(index));
        return std::map<std::string, Jsonator>::insert(std::pair<std::string, Jsonator>(str, Jsonator(this, str)))
            .first->second;
    }

    /**
     * @brief get const json from @p index
     *
     * @param index
     * @return const Jsonator&
     */
    template<typename T>
    const Jsonator& operator[](const T& index) const {
        const_iterator it = find(index);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, index);
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator&
     */
    Jsonator& at(const std::string& key) {
        iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, key);
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     */
    const Jsonator& at(const std::string& key) const {
        const_iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, key);
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator&
     */
    Jsonator& at(const char* key) {
        return at(std::string(key));
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     */
    const Jsonator& at(const char* key) const {
        return at(std::string(key));
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator&
     */
    template<std::size_t Size>
    Jsonator& at(const char (&key)[Size]) {
        return at(std::string(key));
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     */
    template<std::size_t Size>
    const Jsonator& at(const char (&key)[Size]) const {
        return at(std::string(key));
    }

    /**
     * @brief get json from @p index
     *
     * @param index
     * @return Jsonator&
     */
    template<typename T>
    Jsonator& at(const T& index) {
        iterator it = find(index);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, index);
    }

    /**
     * @brief get const json from @p index
     *
     * @param index
     * @return const Jsonator&
     */
    template<typename T>
    const Jsonator& at(const T& index) const {
        const_iterator it = find(index);
        if (it != end()) {
            return it->second;
        }
        throw ChildException(*this, index);
    }

    /**
     * @brief get first json element
     * create then if not exist
     *
     * @return Jsonator&
     */
    Jsonator& front() {
        return at(0);
    }

    /**
     * @brief get first json element
     *
     * @return Jsonator&
     */
    const Jsonator& front() const {
        return at(0);
    }

    /**
     * @brief get last json element
     * create then if not exist
     *
     * @return Jsonator&
     */
    Jsonator& back() {
        if (size() == 0) {
            return at(0);
        }
        else {
            return at(size() - 1);
        }
    }

    /**
     * @brief get last json element
     *
     * @return Jsonator&
     */
    const Jsonator& back() const {
        return at(size() - 1);
    }

    /**
     * @brief insert @p value by @p key
     *
     * @tparam T
     * @param key
     * @param value
     * @return Jsonator&
     */
    template<typename T>
    Jsonator& insert(const std::string& key, const T& value) {
        Jsonator& json = operator[](key);
        json = value;
        return json;
    }

    /**
     * @brief insert @p value by @p index
     *
     * @tparam T
     * @param index
     * @param value
     * @return Jsonator&
     */
    template<typename T>
    Jsonator& insert(const unsigned long& index, const T& value) {
        Jsonator newValue(this, indexToString(index));
        newValue = value;
        // move elements
        for (std::size_t i = size(); i > static_cast<std::size_t>(index); --i) {
            Jsonator& e = at(i - 1);
            e._key = indexToString(i);
            operator[](i)._replace(e);
        }
        operator[](index)._replace(newValue);
        return *this;
    }

    /**
     * @brief add @p value at front
     *
     * @tparam T
     * @param value
     * @return Jsonator&
     */
    template<typename T>
    Jsonator& push_front(const T& value) {
        Jsonator newValue(this, "0");
        newValue = value;
        // move elements
        for (std::size_t i = size(); i > 0; --i) {
            Jsonator& e = at(i - 1);
            e._key = indexToString(i);
            operator[](i)._replace(e);
        }
        operator[](0)._replace(newValue);
        return *this;
    }

    /**
     * @brief add @p value at back
     *
     * @tparam T
     * @param value
     * @return Jsonator&
     */
    template<typename T>
    Jsonator& push_back(const T& value) {
        operator[](size()) = value;
        return *this;
    }

    /**
     * @brief remove first element
     *
     * @return Jsonator&
     */
    Jsonator& pop_front() {
        erase(0);
        return *this;
    }

    /**
     * @brief remove last element
     *
     * @return Jsonator&
     */
    Jsonator& pop_back() {
        erase(size() - 1);
        return *this;
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
     * @brief create string from @p value
     *
     * @tparam T type of value
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
     * @brief create number from @p value
     *
     * @tparam T type of value
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
     * @brief create bool from @p value
     *
     * @tparam T type of value
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

    /**
     * @brief check if type of jsonator is null
     *
     * @return true if type is null else false
     */
    bool isNull() const {
        return _type == NONE;
    }

    /**
     * @brief check if type of jsonator is a object
     *
     * @return true if type is a object else false
     */
    bool isObject() const {
        return _type == OBJECT;
    }

    /**
     * @brief check if type of jsonator is a array
     *
     * @return true if type is a array else false
     */
    bool isArray() const {
        return _type == ARRAY;
    }

    /**
     * @brief check if type of jsonator is a string
     *
     * @return true if type is a string else false
     */
    bool isString() const {
        return _type == STRING;
    }

    /**
     * @brief check if type of jsonator is a number
     *
     * @return true if type is a number else false
     */
    bool isNumber() const {
        return _type == NUMBER;
    }

    /**
     * @brief check if type of jsonator is a boolean
     *
     * @return true if type is a boolean else false
     */
    bool isBoolean() const {
        return _type == BOOLEAN;
    }

    /**
     * @brief check if object contains a @p key
     *
     * @param key
     * @return true if object has key else false
     */
    bool contains(const std::string& key) const {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        return std::map<std::string, Jsonator>::find(key) != std::map<std::string, Jsonator>::end();
    }

    /**
     * @brief check if object contains a @p key
     *
     * @param key
     * @return true if object has key else false
     */
    bool contains(const char* key) const {
        return contains(std::string(key));
    }

    /**
     * @brief check if object contains a @p key
     *
     * @param key
     * @return true if object has key else false
     */
    template<std::size_t Size>
    bool contains(const char (&key)[Size]) const {
        return contains(std::string(key));
    }

    /**
     * @brief check if array contains a @p index
     *
     * @param index
     * @return true if array has index else false
     */
    template<typename T>
    bool contains(const T& index) const {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        return static_cast<std::size_t>(index) < size();
    }

    /**
     * @brief serialize jsonator to string
     *
     * @param indent
     * @param indentCharacter
     * @return std::string
     */
    std::string dump(std::size_t indent = 0, char indentCharacter = ' ') const;

    Jsonator& erase(const std::string& key) {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        iterator it = find(key);
        if (it != end()) {
            std::map<std::string, Jsonator>::erase(key);
            if (empty()) {
                _type = NONE;
            }
        }
        else {
            throw ChildException(*this, key);
        }
        return *this;
    }

    Jsonator& erase(const char* key) {
        return erase(std::string(key));
    }

    template<std::size_t Size>
    Jsonator& erase(const char (&key)[Size]) {
        return erase(std::string(key));
    }

    template<typename T>
    Jsonator& erase(const T& index) {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        iterator it = find(index);
        if (it != end()) {
            // move elements
            for (std::size_t i = index; i < size() - 1; ++i) {
                Jsonator& e = at(i + 1);
                e._key = indexToString(i);
                operator[](i)._replace(e);
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

    /**
     * @brief clear and set at none type
     *
     * @return Jsonator&
     */
    Jsonator& clear() {
        std::map<std::string, Jsonator>::clear();
        _string = "null";
        _number = 0;
        _boolean = false;
        _type = NONE;
        _filename = "";
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
        return *this;
    }

    template<typename T>
    void get(T& ret) const {
        ret = get<T>();
    }

    operator Jsonator*() {
        return this;
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

    /**
     * @brief get json to deque
     *
     * @tparam T
     */
    template<typename T>
    operator std::deque<T>() const {
        std::deque<T> ret;
        for (const_iterator it = begin(); it != end(); ++it) {
            ret.push_back(it->second);
        }
        return ret;
    }

    /**
     * @brief get json to list
     *
     * @tparam T
     */
    template<typename T>
    operator std::list<T>() const {
        std::list<T> ret;
        for (const_iterator it = begin(); it != end(); ++it) {
            ret.push_back(it->second);
        }
        return ret;
    }

    /**
     * @brief get json to map
     *
     * @tparam T key
     * @tparam U value
     */
    template<typename T, typename U>
    operator std::map<T, U>() const {
        std::map<T, U> ret;
        for (const_iterator it = begin(); it != end(); ++it) {
            ret.insert(std::pair<T, U>(it->first, it->second));
        }
        return ret;
    }

    /**
     * @brief get json to queue
     *
     * @tparam T
     */
    template<typename T>
    operator std::queue<T>() const {
        std::queue<T> ret;
        for (const_iterator it = begin(); it != end(); ++it) {
            ret.push(it->second);
        }
        return ret;
    }

    /**
     * @brief get json to set
     *
     * @tparam T
     */
    template<typename T>
    operator std::set<T>() const {
        std::set<T> ret;
        for (const_iterator it = begin(); it != end(); ++it) {
            ret.insert(it->second);
        }
        return ret;
    }

    /**
     * @brief get json to stack
     *
     * @tparam T
     */
    template<typename T>
    operator std::stack<T>() const {
        std::stack<T> ret;
        for (const_iterator it = begin(); it != end(); ++it) {
            ret.push(it->second);
        }
        return ret;
    }

    /**
     * @brief get json to vector
     *
     * @tparam T
     */
    template<typename T>
    operator std::vector<T>() const {
        std::vector<T> ret;
        for (const_iterator it = begin(); it != end(); ++it) {
            ret.push_back(it->second);
        }
        return ret;
    }

    /**
     * @brief get number to T
     *
     * @tparam T
     */
    template<typename T>
    operator T() const {
        return getNumber();
    }

    /**
     * @brief open and parse @p filename
     *
     * @param filename
     * @param comment active for skip the comment at parse
     * @param additionalNext active for skip the additional ',' at parse
     * @return Jsonator object
     */
    static Jsonator parseFile(const char* filename, bool comment = true, bool additionalNext = true);

    /**
     * @brief parse @p stream
     *
     * @param stream
     * @param comment active for skip the comment at parse
     * @param additionalNext active for skip the additional ',' at parse
     * @return Jsonator
     */
    static Jsonator parseStream(std::istream& stream, bool comment = true, bool additionalNext = true) {
        return _parseStream(stream, std::string(), comment, additionalNext);
    }

    /**
     * @brief parse @p str
     *
     * @param str
     * @param comment active for skip the comment at parse
     * @param additionalNext active for skip the additional ',' at parse
     * @return Jsonator
     */
    static Jsonator parseString(const std::string& str, bool comment = true, bool additionalNext = true) {
        std::istringstream iss(str);
        return parseStream(iss, comment, additionalNext);
    }

    /**
     * @brief parse @p data with @p size
     *
     * @param data
     * @param size
     * @param comment active for skip the comment at parse
     * @param additionalNext active for skip the additional ',' at parse
     * @return Jsonator
     */
    static Jsonator parseData(const void* data, std::size_t size, bool comment = true, bool additionalNext = true) {
        return parseString(std::string(static_cast<const char*>(data), size), comment, additionalNext);
    }

    const std::string& getFilename() const {
        return _filename;
    }

  private:
    static void ultostr(char** pstr, unsigned long l) {
        if (l < 10UL) {
            **pstr = '0' + l;
            ++(*pstr);
        }
        else {
            ultostr(pstr, l / 10UL);
            **pstr = '0' + (l % 10UL);
            ++(*pstr);
        }
    }

    static std::string indexToString(unsigned long index) {
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
            ultostr(&tmp, index);
            return std::string(str, tmp - str);
        }
    }

    static Jsonator _parseStream(std::istream& stream, const std::string& filename, bool comment, bool additionalNext);

    void _replaceParent(Jsonator& json);
    void _replace(const Jsonator& json);

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

// #include "mblet/jsonator.h"


#include <cctype>  // ::isdigit, ::isspace
#include <cstdlib> // ::strtod
#include <cstring> // ::strncmp
#include <fstream> // std::ifstream
#include <iomanip> // std::setprecision
#include <limits>  // std::numeric_limits

namespace mblet {

inline Jsonator::Jsonator() :
    std::map<std::string, Jsonator>(),
    _parent(NULL),
    _key(""),
    _string("null"),
    _number(0),
    _boolean(false),
    _type(NONE),
    _filename("") {}

inline Jsonator::Jsonator(const Jsonator* parent, const std::string& key) :
    std::map<std::string, Jsonator>(),
    _parent(parent),
    _key(key),
    _string("null"),
    _number(0),
    _boolean(false),
    _type(NONE),
    _filename("") {}

inline Jsonator::Jsonator(const Jsonator& rhs) :
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

inline Jsonator::~Jsonator() {}

inline void Jsonator::operator=(const Jsonator& json) {
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

inline Jsonator Jsonator::parseFile(const char* filename, bool comment, bool next) {
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

inline static void s_typeDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter, std::size_t index = 0);

inline static void s_objectDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter, std::size_t index) {
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

inline static void s_arrayDump(std::ostream& oss, const Jsonator& json, std::size_t indent, char indentCharacter, std::size_t index) {
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

inline std::string Jsonator::dump(std::size_t indent, char indentCharacter) const {
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

inline static bool s_parseType(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator& json);

inline static void s_parseArray(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator& json) {
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

inline static void s_parseObject(const JsonatorParseInfo& info, const std::string& str, std::size_t& i, Jsonator& json) {
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

inline static void s_replaceCommentBySpace(std::string& str) {
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

inline static std::string s_streamToStr(JsonatorParseInfo& info, std::istream& stream) {
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

inline Jsonator Jsonator::_parseStream(std::istream& stream, const std::string& filename, bool comment, bool additionalNext) {
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

inline void Jsonator::_replaceParent(Jsonator& json) {
    for (Jsonator::iterator it = json.begin(); it != json.end(); ++it) {
        it->second._parent = &json;
        _replaceParent(it->second);
    }
}

inline void Jsonator::_replace(const Jsonator& json) {
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
