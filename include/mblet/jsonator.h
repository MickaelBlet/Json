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
class Jsonator {
  public:
    typedef std::vector<Jsonator>::const_iterator array_const_iterator;
    typedef std::vector<Jsonator>::iterator array_iterator;
    typedef std::map<std::string, Jsonator>::const_iterator object_const_iterator;
    typedef std::map<std::string, Jsonator>::iterator object_iterator;

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

      protected:
        static void jsonToPath(std::ostream& oss, const Jsonator& json) {
            std::stack<const Jsonator*> jsons;
            const Jsonator* pJson = json._parent;
            while (pJson != NULL) {
                jsons.push(pJson);
                pJson = pJson->_parent;
            }
            while (!jsons.empty()) {
                if (jsons.top()->_parent) {
                    if (jsons.top()->_parent->isObject()) {
                        oss << "[\"" << jsons.top()->_objectKey << "\"]";
                    }
                    else if (jsons.top()->_parent && jsons.top()->_parent->isArray()) {
                        oss << '[' << jsons.top()->_arrayIndex << ']';
                    }
                }
                jsons.pop();
            }
            if (json._parent && json._parent->isObject()) {
                oss << "[\"" << json._objectKey << "\"]";
            }
            if (json._parent && json._parent->isArray()) {
                oss << '[' << json._arrayIndex << ']';
            }
        }

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
        const char* ret = NULL;
        switch (type) {
            case NONE:
                ret = "NONE";
                break;
            case OBJECT:
                ret = "OBJECT";
                break;
            case ARRAY:
                ret = "ARRAY";
                break;
            case STRING:
                ret = "STRING";
                break;
            case NUMBER:
                ret = "NUMBER";
                break;
            case BOOLEAN:
                ret = "BOOLEAN";
                break;
        }
        return ret;
    }

    /**
     * @brief override operator stream for print value
     *
     * @param os
     * @param json
     * @return std::ostream&
     */
    friend std::ostream& operator<<(std::ostream& os, const Jsonator& json) {
        switch (json.getType()) {
            case OBJECT:
            case ARRAY:
                os << json.dump();
                break;
            case NONE:
                os << "null";
                break;
            case STRING:
                os << json.getString();
                break;
            case NUMBER:
                os << json.getNumber();
                break;
            case BOOLEAN:
                os << json.getBoolean();
                break;
        }
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
     * @throw AccessException if type is not none or not a string
     */
    void operator=(const std::string& value) {
        newString(value);
    }

    /**
     * @brief set value from str
     *
     * @param value : new value
     * @throw AccessException if type is not none or not a string
     */
    void operator=(const char* value) {
        newString(value);
    }

    /**
     * @brief set value from str
     *
     * @param value : new value
     * @throw AccessException if type is not none or not a string
     */
    template<std::size_t Size>
    void operator=(const char (&value)[Size]) {
        newString(std::string(value));
    }

    /**
     * @brief set value from bool
     *
     * @param value : new value
     * @throw AccessException if type is not none or not a boolean
     */
    void operator=(const bool& value) {
        newBoolean(value);
    }

    /**
     * @brief set value from deque
     *
     * @tparam T
     * @param value
     * @throw AccessException if type is not none or not a array
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
     * @throw AccessException if type is not none or not a array
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
     * @throw AccessException if type is not none or not a array
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
     * @throw AccessException if type is not none or not a array
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
     * @throw AccessException if type is not none or not a array
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
     * @throw AccessException if type is not none or not a array
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
     * @throw AccessException if type is not none or not a array
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
     * @throw AccessException if type is not none or not a number
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
     * @throw AccessException if type is not a object
     */
    object_iterator find(const std::string& key) {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        return _object.find(key);
    }

    /**
     * @brief find by key
     *
     * @param key
     * @return const_iterator
     * @throw AccessException if type is not a object
     */
    object_const_iterator find(const std::string& key) const {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        return _object.find(key);
    }

    /**
     * @brief find by key
     *
     * @param key
     * @return iterator
     * @throw AccessException if type is not a object
     */
    object_iterator find(const char* key) {
        return find(std::string(key));
    }

    /**
     * @brief find by key
     *
     * @param key
     * @return const_iterator
     * @throw AccessException if type is not a object
     */
    object_const_iterator find(const char* key) const {
        return find(std::string(key));
    }

    /**
     * @brief find by key
     *
     * @tparam Size
     * @return iterator
     * @throw AccessException if type is not a object
     */
    template<std::size_t Size>
    object_iterator find(const char (&key)[Size]) {
        return find(std::string(key));
    }

    /**
     * @brief find by key
     *
     * @tparam Size
     * @return const_iterator
     * @throw AccessException if type is not a object
     */
    template<std::size_t Size>
    object_const_iterator find(const char (&key)[Size]) const {
        return find(std::string(key));
    }

    /**
     * @brief find by index
     *
     * @tparam T
     * @param index
     * @return iterator
     * @throw AccessException if type is not a array
     */
    template<typename T>
    array_iterator find(const T& index) {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        if (static_cast<std::size_t>(index) < _array.size()) {
            return _array.begin() + index;
        }
        else {
            return _array.end();
        }
    }

    /**
     * @brief find by index
     *
     * @tparam T
     * @param index
     * @return const_iterator
     * @throw AccessException if type is not a array
     */
    template<typename T>
    array_const_iterator find(const T& index) const {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        if (static_cast<std::size_t>(index) < _array.size()) {
            return _array.begin() + index;
        }
        else {
            return _array.end();
        }
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator& data associated with the key or if the key does not exist,
     * a json object with that key is created using default values, which is then returned
     * @throw AccessException if type is not a null and not a object
     */
    Jsonator& operator[](const std::string& key) {
        if (_type == NONE) {
            _type = OBJECT;
        }
        object_iterator it = find(key);
        if (it != _object.end()) {
            return it->second;
        }
        return _object.insert(std::pair<std::string, Jsonator>(key, Jsonator(this, key)))
            .first->second;
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
     */
    const Jsonator& operator[](const std::string& key) const {
        object_const_iterator it = find(key);
        if (it != _object.end()) {
            return it->second;
        }
        throw ChildException(*this, key);
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator& data associated with the key or if the key does not exist,
     * a json object with that key is created using default values, which is then returned
     * @throw AccessException if type is not a null and not a object
     */
    Jsonator& operator[](const char* key) {
        return operator[](std::string(key));
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
     */
    const Jsonator& operator[](const char* key) const {
        return operator[](std::string(key));
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator& data associated with the key or if the key does not exist,
     * a json object with that key is created using default values, which is then returned
     * @throw AccessException if type is not a null and not a object
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
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
     */
    template<std::size_t Size>
    const Jsonator& operator[](const char (&key)[Size]) const {
        return operator[](std::string(key));
    }

    /**
     * @brief get json from @p index if the lowers indexies not exists create of null object
     *
     * @param index
     * @return Jsonator& data associated with the index or if the index does not exist,
     * a json object with that index is created using default values, which is then returned
     * @throw AccessException if type is not a null and not a array
     */
    template<typename T>
    Jsonator& operator[](const T& index) {
        if (_type == NONE) {
            _type = ARRAY;
        }
        array_iterator it = find(index);
        if (it != _array.end()) {
            return *it;
        }
        while (_array.size() < static_cast<std::size_t>(index)) {
            _array.push_back(Jsonator(this, _array.size()));
        }
        _array.push_back(Jsonator(this, index));
        return _array.back();
    }

    /**
     * @brief get const json from @p index
     *
     * @param index
     * @return const Jsonator&
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    template<typename T>
    const Jsonator& operator[](const T& index) const {
        array_const_iterator it = find(index);
        if (it != _array.end()) {
            return *it;
        }
        throw ChildException(*this, index);
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator&
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
     */
    Jsonator& at(const std::string& key) {
        object_iterator it = find(key);
        if (it != _object.end()) {
            return it->second;
        }
        throw ChildException(*this, key);
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
     */
    const Jsonator& at(const std::string& key) const {
        object_const_iterator it = find(key);
        if (it != _object.end()) {
            return it->second;
        }
        throw ChildException(*this, key);
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator&
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
     */
    Jsonator& at(const char* key) {
        return at(std::string(key));
    }

    /**
     * @brief get const json from @p key
     *
     * @param key
     * @return const Jsonator&
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
     */
    const Jsonator& at(const char* key) const {
        return at(std::string(key));
    }

    /**
     * @brief get json from @p key
     *
     * @param key
     * @return Jsonator&
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
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
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
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
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    template<typename T>
    Jsonator& at(const T& index) {
        array_iterator it = find(index);
        if (it != _array.end()) {
            return *it;
        }
        throw ChildException(*this, index);
    }

    /**
     * @brief get const json from @p index
     *
     * @param index
     * @return const Jsonator&
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    template<typename T>
    const Jsonator& at(const T& index) const {
        array_const_iterator it = find(index);
        if (it != _array.end()) {
            return *it;
        }
        throw ChildException(*this, index);
    }

    /**
     * @brief Check if json object is empty
     *
     * @return true if is empty
     */
    bool empty() const {
        if (_type == ARRAY) {
            return _array.empty();
        }
        else if (_type == OBJECT) {
            return _object.empty();
        }
        else {
            return true;
        }
    }

    /**
     * @brief get the const iterator of begin object
     *
     * @return object_const_iterator
     */
    object_const_iterator object_begin() const {
        if (_type != NONE && _type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        return _object.begin();
    }

    /**
     * @brief get the iterator of begin object
     *
     * @return object_const_iterator
     */
    object_iterator object_begin() {
        if (_type != NONE && _type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        return _object.begin();
    }

    /**
     * @brief get the const iterator of begin array
     *
     * @return object_const_iterator
     */
    array_const_iterator array_begin() const {
        if (_type != NONE && _type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        return _array.begin();
    }

    /**
     * @brief get the iterator of begin array
     *
     * @return object_const_iterator
     */
    array_iterator array_begin() {
        if (_type != NONE && _type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        return _array.begin();
    }

    /**
     * @brief get the const iterator of end object
     *
     * @return object_const_iterator
     */
    object_const_iterator object_end() const {
        if (_type != NONE && _type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        return _object.end();
    }

    /**
     * @brief get the iterator of end object
     *
     * @return object_const_iterator
     */
    object_iterator object_end() {
        if (_type != NONE && _type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        return _object.end();
    }

    /**
     * @brief get the const iterator of end array
     *
     * @return object_const_iterator
     */
    array_const_iterator array_end() const {
        if (_type != NONE && _type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        return _array.end();
    }

    /**
     * @brief get the iterator of end array
     *
     * @return object_const_iterator
     */
    array_iterator array_end() {
        if (_type != NONE && _type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        return _array.end();
    }

    /**
     * @brief get the size of object or array
     *
     * @return std::size_t
     */
    std::size_t size() const {
        if (_type == ARRAY) {
            return _array.size();
        }
        else if (_type == OBJECT) {
            return _object.size();
        }
        return 0;
    }

    /**
     * @brief get first json element
     *
     * @return Jsonator&
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    Jsonator& front() {
        return at(0);
    }

    /**
     * @brief get first json element
     *
     * @return Jsonator&
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    const Jsonator& front() const {
        return at(0);
    }

    /**
     * @brief get last json element
     * create then if not exist
     *
     * @return Jsonator&
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    Jsonator& back() {
        if (_array.size() == 0) {
            return at(0);
        }
        else {
            return at(_array.size() - 1);
        }
    }

    /**
     * @brief get last json element
     *
     * @return Jsonator&
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    const Jsonator& back() const {
        return at(_array.size() - 1);
    }

    /**
     * @brief insert @p value by @p key
     *
     * @tparam T
     * @param key
     * @param value
     * @return Jsonator&
     * @throw AccessException if type is not a null and not a object
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
     * @throw AccessException if type is not a null and not a array
     */
    template<typename T>
    Jsonator& insert(const unsigned long& index, const T& value) {
        Jsonator newValue(this, index);
        newValue = value;
        // move elements
        for (std::size_t i = _array.size(); i > static_cast<std::size_t>(index); --i) {
            Jsonator& n = operator[](i);
            Jsonator& e = _array[i - 1];
            e._arrayIndex = i;
            n._replace(e);
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
     * @throw AccessException if type is not a null and not a array
     */
    template<typename T>
    Jsonator& push_front(const T& value) {
        Jsonator newValue(this, 0);
        newValue = value;
        // move elements
        for (std::size_t i = _array.size(); i > 0; --i) {
            Jsonator& n = operator[](i);
            Jsonator& e = _array[i - 1];
            e._arrayIndex = i;
            n._replace(e);
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
     * @throw AccessException if type is not a null and not a array
     */
    template<typename T>
    Jsonator& push_back(const T& value) {
        operator[](_array.size()) = value;
        return *this;
    }

    /**
     * @brief remove first element
     *
     * @return Jsonator&
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    Jsonator& pop_front() {
        erase(0);
        return *this;
    }

    /**
     * @brief remove last element
     *
     * @return Jsonator&
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    Jsonator& pop_back() {
        erase(_array.size() - 1);
        return *this;
    }

    /**
     * @brief create object
     * @throw AccessException if type is not a null and not a object
     */
    void newObject() {
        if (_type != NONE && _type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        _type = OBJECT;
    }

    /**
     * @brief create array
     * @throw AccessException if type is not a null and not a array
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
     * @throw AccessException if type is not a null and not a string
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
     * @throw AccessException if type is not a null and not a number
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
     * @throw AccessException if type is not a null and not a boolean
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
     * @throw AccessException if type is not a null
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
     * @throw AccessException if type is not a object
     */
    bool contains(const std::string& key) const {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        return _object.find(key) != _object.end();
    }

    /**
     * @brief check if object contains a @p key
     *
     * @param key
     * @return true if object has key else false
     * @throw AccessException if type is not a object
     */
    bool contains(const char* key) const {
        return contains(std::string(key));
    }

    /**
     * @brief check if object contains a @p key
     *
     * @param key
     * @return true if object has key else false
     * @throw AccessException if type is not a object
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
     * @throw AccessException if type is not a array
     */
    template<typename T>
    bool contains(const T& index) const {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        return static_cast<std::size_t>(index) < _array.size();
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
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
     */
    Jsonator& erase(const std::string& key) {
        if (_type != OBJECT) {
            throw AccessException(*this, "is not a object");
        }
        object_iterator it = find(key);
        if (it != _object.end()) {
            _object.erase(key);
            if (_object.empty()) {
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
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
     */
    Jsonator& erase(const char* key) {
        return erase(std::string(key));
    }

    /**
     * @brief remove the @p key
     *
     * @param key
     * @return Jsonator&
     * @throw AccessException if type is not a object
     * @throw ChildException if key is not exists
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
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    template<typename T>
    Jsonator& erase(const T& index) {
        if (_type != ARRAY) {
            throw AccessException(*this, "is not a array");
        }
        array_iterator it = find(index);
        if (it != _array.end()) {
            // move elements
            for (std::size_t i = index; i < _array.size() - 1; ++i) {
                Jsonator& e = at(i + 1);
                e._arrayIndex = i;
                operator[](i)._replace(e);
            }
            _array.erase(_array.begin() + (_array.size() - 1));
            if (_array.empty()) {
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
        _object.clear();
        _array.clear();
        _string = "";
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

    const std::size_t& getIndex() const {
        return _arrayIndex;
    }

    /**
     * @brief get the key of json object
     *
     * @return const std::string&
     */
    const std::string& getKey() const {
        return _objectKey;
    }

    /**
     * @brief get the string of json object
     *
     * @return const std::string&
     * @throw AccessException if json type is not a string
     */
    const std::string& getString() const {
        if (_type != STRING) {
            throw AccessException(*this, "is not a string");
        }
        return _string;
    }

    /**
     * @brief get the number of json object
     *
     * @return const double&
     * @throw AccessException if json type is not a number
     */
    const double& getNumber() const {
        if (_type != NUMBER) {
            throw AccessException(*this, "is not a number");
        }
        return _number;
    }

    /**
     * @brief get the boolean of json object
     *
     * @return const bool&
     * @throw AccessException if json type is not a boolean
     */
    const bool& getBoolean() const {
        if (_type != BOOLEAN) {
            throw AccessException(*this, "is not a boolean");
        }
        return _boolean;
    }

    /**
     * @brief Get the Type object
     *
     * @return const Type&
     */
    const Type& getType() const {
        return _type;
    }

    /**
     * @brief get the @c T of json object
     *
     * @tparam T
     * @return T
     */
    template<typename T>
    T get() const {
        return *this;
    }

    /**
     * @brief get the @p T of json object
     *
     * @tparam T
     * @param ret
     */
    template<typename T>
    void get(T& ret) const {
        ret = get<T>();
    }

    /**
     * @brief string construct
     *
     * @return const std::string&
     * @throw AccessException if json type is not a string
     */
    operator const std::string&() const {
        return getString();
    }

    /**
     * @brief string construct
     *
     * @return const char*
     * @throw AccessException if json type is not a string
     */
    operator const char*() const {
        return getString().c_str();
    }

    /**
     * @brief bool construct
     *
     * @return const bool&
     * @throw AccessException if json type is not a boolean
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
        if (_type == ARRAY) {
            for (std::size_t i = 0; i < _array.size(); ++i) {
                ret.push_back(_array[i]);
            }
        }
        else if (_type == OBJECT) {
            for (std::map<std::string, Jsonator>::const_iterator it = _object.begin(); it != _object.end(); ++it) {
                ret.push_back(it->second);
            }
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
        if (_type == ARRAY) {
            for (std::size_t i = 0; i < _array.size(); ++i) {
                ret.push_back(_array[i]);
            }
        }
        else if (_type == OBJECT) {
            for (std::map<std::string, Jsonator>::const_iterator it = _object.begin(); it != _object.end(); ++it) {
                ret.push_back(it->second);
            }
        }
        return ret;
    }

    /**
     * @brief get json to map
     *
     * @tparam T
     * @return std::map<std::string, T>
     */
    template<typename T>
    operator std::map<std::string, T>() const {
        std::map<std::string, T> ret;
        for (std::map<std::string, Jsonator>::const_iterator it = _object.begin(); it != _object.end(); ++it) {
            ret.insert(std::pair<std::string, T>(it->first, it->second));
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
        for (std::size_t i = 0; i < _array.size(); ++i) {
            ret.insert(std::pair<T, U>(i, _array[i]));
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
        if (_type == ARRAY) {
            for (std::size_t i = 0; i < _array.size(); ++i) {
                ret.push(_array[i]);
            }
        }
        else if (_type == OBJECT) {
            for (std::map<std::string, Jsonator>::const_iterator it = _object.begin(); it != _object.end(); ++it) {
                ret.push(it->second);
            }
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
        if (_type == ARRAY) {
            for (std::size_t i = 0; i < _array.size(); ++i) {
                ret.insert(_array[i]);
            }
        }
        else if (_type == OBJECT) {
            for (std::map<std::string, Jsonator>::const_iterator it = _object.begin(); it != _object.end(); ++it) {
                ret.insert(it->second);
            }
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
        if (_type == ARRAY) {
            for (std::size_t i = 0; i < _array.size(); ++i) {
                ret.push(_array[i]);
            }
        }
        else if (_type == OBJECT) {
            for (std::map<std::string, Jsonator>::const_iterator it = _object.begin(); it != _object.end(); ++it) {
                ret.push(it->second);
            }
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
        if (_type == ARRAY) {
            for (std::size_t i = 0; i < _array.size(); ++i) {
                ret.push_back(_array[i]);
            }
        }
        else if (_type == OBJECT) {
            for (std::map<std::string, Jsonator>::const_iterator it = _object.begin(); it != _object.end(); ++it) {
                ret.push_back(it->second);
            }
        }
        return ret;
    }

    /**
     * @brief get number to T
     *
     * @tparam T
     * @throw AccessException if json type is not a number
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
     * @throw ParseException by json format
     */
    static Jsonator parseFile(const char* filename, bool comment = true, bool additionalNext = true);

    /**
     * @brief parse @p stream
     *
     * @param stream
     * @param comment active for skip the comment at parse
     * @param additionalNext active for skip the additional ',' at parse
     * @return Jsonator
     * @throw ParseException by json format
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
     * @throw ParseException by json format
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
     * @throw ParseException by json format
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
     * @param index
     */
    Jsonator(const Jsonator* parent, const std::size_t& index);

    /**
     * @brief Construct a new Jsonator object
     *
     * @param parent
     * @param key
     */
    Jsonator(const Jsonator* parent, const std::string& key);

    static Jsonator _parseStream(std::istream& stream, const std::string& filename, bool comment, bool additionalNext);

    void _replaceParent(Jsonator& json);
    void _replace(const Jsonator& json);

    const Jsonator* _parent;
    std::vector<Jsonator> _array;
    std::size_t _arrayIndex;
    std::map<std::string, Jsonator> _object;
    std::string _objectKey;
    std::string _string;
    double _number;
    bool _boolean;
    Type _type;
    std::string _filename;
};

} // namespace mblet

#endif // _MBLET_JSONATOR_H_