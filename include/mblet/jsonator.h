/**
 * jsonator.h
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
     * @throw mblet::Jsonator::AccessException if type is not a object
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
     * @throw mblet::Jsonator::AccessException if type is not a object
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
     * @throw mblet::Jsonator::AccessException if type is not a object
     */
    iterator find(const char* key) {
        return find(std::string(key));
    }

    /**
     * @brief find by key
     *
     * @param key
     * @return const_iterator
     * @throw mblet::Jsonator::AccessException if type is not a object
     */
    const_iterator find(const char* key) const {
        return find(std::string(key));
    }

    /**
     * @brief find by key
     *
     * @tparam Size
     * @return iterator
     * @throw mblet::Jsonator::AccessException if type is not a object
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
     * @throw mblet::Jsonator::AccessException if type is not a object
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
     * @throw mblet::Jsonator::AccessException if type is not a array
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
     * @throw mblet::Jsonator::AccessException if type is not a array
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
     * @return Jsonator& data associated with the key or if the key does not exist,
     * a json object with that key is created using default values, which is then returned
     * @throw mblet::Jsonator::AccessException if type is not a null and not a object
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
     * @throw mblet::Jsonator::AccessException if type is not a object
     * @throw mblet::Jsonator::ChildException if key is not exists
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
     * @throw mblet::Jsonator::AccessException if type is not a object
     * @throw mblet::Jsonator::ChildException if key is not exists
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
     * @throw mblet::Jsonator::AccessException if type is not a object
     * @throw mblet::Jsonator::ChildException if key is not exists
     */
    template<std::size_t Size>
    const Jsonator& operator[](const char (&key)[Size]) const {
        return operator[](std::string(key));
    }

    /**
     * @brief get json from @p index if the lowers indexies not exists create of null object
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
     * @throw mblet::Jsonator::AccessException if type is not a array
     * @throw mblet::Jsonator::ChildException if index is not exists
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
     * @throw mblet::Jsonator::AccessException if type is not a object
     * @throw mblet::Jsonator::ChildException if key is not exists
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
     * @throw mblet::Jsonator::AccessException if type is not a object
     * @throw mblet::Jsonator::ChildException if key is not exists
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
     * @throw mblet::Jsonator::AccessException if type is not a object
     * @throw mblet::Jsonator::ChildException if key is not exists
     */
    Jsonator& at(const char* key) {
        return at(std::string(key));
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     * @throw mblet::Jsonator::AccessException if type is not a object
     * @throw mblet::Jsonator::ChildException if key is not exists
     */
    const Jsonator& at(const char* key) const {
        return at(std::string(key));
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator&
     * @throw mblet::Jsonator::AccessException if type is not a object
     * @throw mblet::Jsonator::ChildException if key is not exists
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
     * @throw mblet::Jsonator::AccessException if type is not a object
     * @throw mblet::Jsonator::ChildException if key is not exists
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
     * @throw mblet::Jsonator::AccessException if type is not a array
     * @throw mblet::Jsonator::ChildException if index is not exists
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
     * @throw mblet::Jsonator::AccessException if type is not a array
     * @throw mblet::Jsonator::ChildException if index is not exists
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
     *
     * @return Jsonator&
     * @throw mblet::Jsonator::AccessException if type is not a array
     * @throw mblet::Jsonator::ChildException if index is not exists
     */
    Jsonator& front() {
        return at(0);
    }

    /**
     * @brief get first json element
     *
     * @return Jsonator&
     * @throw mblet::Jsonator::AccessException if type is not a array
     * @throw mblet::Jsonator::ChildException if index is not exists
     */
    const Jsonator& front() const {
        return at(0);
    }

    /**
     * @brief get last json element
     * create then if not exist
     *
     * @return Jsonator&
     * @throw mblet::Jsonator::AccessException if type is not a array
     * @throw mblet::Jsonator::ChildException if index is not exists
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
     * @throw mblet::Jsonator::AccessException if type is not a array
     * @throw mblet::Jsonator::ChildException if index is not exists
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
     * @throw mblet::Jsonator::AccessException if type is not a array
     * @throw mblet::Jsonator::ChildException if index is not exists
     */
    Jsonator& pop_front() {
        erase(0);
        return *this;
    }

    /**
     * @brief remove last element
     *
     * @return Jsonator&
     * @throw mblet::Jsonator::AccessException if type is not a array
     * @throw mblet::Jsonator::ChildException if index is not exists
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

    /**
     * @brief remove the @p key
     *
     * @param key
     * @return Jsonator&
     */
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

    /**
     * @brief remove the @p key
     *
     * @param key
     * @return Jsonator&
     */
    Jsonator& erase(const char* key) {
        return erase(std::string(key));
    }

    /**
     * @brief remove the @p key
     *
     * @param key
     * @return Jsonator&
     */
    template<std::size_t Size>
    Jsonator& erase(const char (&key)[Size]) {
        return erase(std::string(key));
    }

    /**
     * @brief remove the @p index
     *
     * @param index
     * @return Jsonator&
     */
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

    /**
     * @brief get the parent json object
     *
     * @return const Jsonator* parent else NULL
     */
    const Jsonator* getParent() const {
        return _parent;
    }

    /**
     * @brief get the key of json object
     *
     * @return const std::string&
     */
    const std::string& getKey() const {
        return _key;
    }

    /**
     * @brief get the string of string json object
     *
     * @return const std::string&
     * @throw mblet::Jsonator::AccessException if json type is not string
     */
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

    /**
     * @brief string construct
     *
     * @return const std::string&
     */
    operator const std::string&() const {
        return getString();
    }

    /**
     * @brief string construct
     *
     * @return const char*
     */
    operator const char*() const {
        return getString().c_str();
    }

    /**
     * @brief bool construct
     *
     * @return const bool&
     */
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

    /**
     * @brief get name of file after used parseFile
     *
     * @return const std::string&
     */
    const std::string& getFilename() const {
        return _filename;
    }

  private:
    /**
     * @brief Construct a new Jsonator object
     *
     * @param parent
     * @param key
     */
    Jsonator(const Jsonator* parent, const std::string& key);

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
        static const std::string numbers[] = {
            "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "10", "11", "12", "13", "14", "15", "16",
            "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33",
            "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50",
            "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65", "66", "67",
            "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83", "84",
            "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99"};
        if (index < 100) {
            return numbers[index];
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