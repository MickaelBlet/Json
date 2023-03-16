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
            if (!_filename.empty()) {
                oss << _filename << ':';
            }
            oss << " (" << _message << ").";
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
            if (!_filename.empty()) {
                oss << _filename << ':';
            }
            oss << _line << ':' << _column << " (" << _message << ").";
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
            oss << _message << " '" << index << "'.";
            _what = oss.str();
        }
        ChildException(const Jsonator& json, const std::string& child) :
            AccessException(json, "has not a child"),
            _child(child) {
            std::ostringstream oss("");
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
    enum EType {
        NONE_TYPE = 0,
        OBJECT_TYPE,
        ARRAY_TYPE,
        STRING_TYPE,
        NUMBER_TYPE,
        BOOLEAN_TYPE
    };

    /**
     * @brief Get the Str From EType
     *
     * @param type
     * @return const char*
     */
    static const char* getStrFromType(const EType& type) {
        const char* ret = NULL;
        switch (type) {
            case NONE_TYPE:
                ret = "NONE_TYPE";
                break;
            case OBJECT_TYPE:
                ret = "OBJECT_TYPE";
                break;
            case ARRAY_TYPE:
                ret = "ARRAY_TYPE";
                break;
            case STRING_TYPE:
                ret = "STRING_TYPE";
                break;
            case NUMBER_TYPE:
                ret = "NUMBER_TYPE";
                break;
            case BOOLEAN_TYPE:
                ret = "BOOLEAN_TYPE";
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
            case OBJECT_TYPE:
            case ARRAY_TYPE:
                os << json.dump();
                break;
            case NONE_TYPE:
                os << "null";
                break;
            case STRING_TYPE:
                os << json.getString();
                break;
            case NUMBER_TYPE:
                os << json.getNumber();
                break;
            case BOOLEAN_TYPE:
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
    Jsonator& operator=(const Jsonator& json);

    /**
     * @brief set value from str
     *
     * @param value : new value
     * @throw AccessException if type is not none or not a string
     */
    Jsonator& operator=(const std::string& value) {
        newString(value);
        return *this;
    }

    /**
     * @brief set value from str
     *
     * @param value : new value
     * @throw AccessException if type is not none or not a string
     */
    Jsonator& operator=(const char* value) {
        newString(value);
        return *this;
    }

    /**
     * @brief set value from str
     *
     * @param value : new value
     * @throw AccessException if type is not none or not a string
     */
    template<std::size_t Size>
    Jsonator& operator=(const char (&value)[Size]) {
        newString(std::string(value));
        return *this;
    }

    /**
     * @brief set value from bool
     *
     * @param value : new value
     * @throw AccessException if type is not none or not a boolean
     */
    Jsonator& operator=(const bool& value) {
        newBoolean(value);
        return *this;
    }

    /**
     * @brief set value from deque
     *
     * @tparam T
     * @param value
     * @throw AccessException if type is not none or not a array
     */
    template<typename T>
    Jsonator& operator=(const std::deque<T>& value) {
        typename std::deque<T>::const_iterator it;
        for (it = value.begin(); it != value.end(); ++it) {
            push_back(*it);
        }
        return *this;
    }

    /**
     * @brief set value from list
     *
     * @tparam T
     * @param value
     * @throw AccessException if type is not none or not a array
     */
    template<typename T>
    Jsonator& operator=(const std::list<T>& value) {
        typename std::list<T>::const_iterator it;
        for (it = value.begin(); it != value.end(); ++it) {
            push_back(*it);
        }
        return *this;
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
    Jsonator& operator=(const std::map<T, U>& value) {
        typename std::map<T, U>::const_iterator it;
        for (it = value.begin(); it != value.end(); ++it) {
            operator[](it->first) = it->second;
        }
        return *this;
    }

    /**
     * @brief set value from queue
     *
     * @tparam T
     * @param value
     * @throw AccessException if type is not none or not a array
     */
    template<typename T>
    Jsonator& operator=(const std::queue<T>& value) {
        std::queue<T> copy = value;
        while (!copy.empty()) {
            push_back(copy.front());
            copy.pop();
        }
        return *this;
    }

    /**
     * @brief set value from set
     *
     * @tparam T
     * @param value
     * @throw AccessException if type is not none or not a array
     */
    template<typename T>
    Jsonator& operator=(const std::set<T>& value) {
        typename std::set<T>::const_iterator it;
        for (it = value.begin(); it != value.end(); ++it) {
            push_back(*it);
        }
        return *this;
    }

    /**
     * @brief set value from stack
     *
     * @tparam T
     * @param value
     * @throw AccessException if type is not none or not a array
     */
    template<typename T>
    Jsonator& operator=(const std::stack<T>& value) {
        std::stack<T> copy = value;
        while (!copy.empty()) {
            push_back(copy.top());
            copy.pop();
        }
        return *this;
    }

    /**
     * @brief set value from vector
     *
     * @tparam T
     * @param value
     * @throw AccessException if type is not none or not a array
     */
    template<typename T>
    Jsonator& operator=(const std::vector<T>& value) {
        typename std::vector<T>::const_iterator it;
        for (it = value.begin(); it != value.end(); ++it) {
            push_back(*it);
        }
        return *this;
    }

    /**
     * @brief set value from number
     *
     * @tparam T
     * @param value
     * @throw AccessException if type is not none or not a number
     */
    template<typename T>
    Jsonator& operator=(const T& value) {
        newNumber(value);
        return *this;
    }

    /**
     * @brief find by key
     *
     * @param key
     * @return iterator
     * @throw AccessException if type is not a object
     */
    object_iterator find(const std::string& key) {
        if (!isObject()) {
            throw AccessException(*this, "is not a object");
        }
        return _value.object->find(key);
    }

    /**
     * @brief find by key
     *
     * @param key
     * @return const_iterator
     * @throw AccessException if type is not a object
     */
    object_const_iterator find(const std::string& key) const {
        if (!isObject()) {
            throw AccessException(*this, "is not a object");
        }
        return _value.object->find(key);
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
        if (!isArray()) {
            throw AccessException(*this, "is not a array");
        }
        if (static_cast<std::size_t>(index) < _value.array->size()) {
            return _value.array->begin() + index;
        }
        else {
            return _value.array->end();
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
        if (!isArray()) {
            throw AccessException(*this, "is not a array");
        }
        if (static_cast<std::size_t>(index) < _value.array->size()) {
            return _value.array->begin() + index;
        }
        else {
            return _value.array->end();
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
        if (isNull()) {
            newObject();
        }
        object_iterator it = find(key);
        if (it != _value.object->end()) {
            return it->second;
        }
        return _value.object->insert(std::pair<std::string, Jsonator>(key, Jsonator())).first->second;
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
        if (it != _value.object->end()) {
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
        if (isNull()) {
            newArray();
        }
        array_iterator it = find(index);
        if (it != _value.array->end()) {
            return *it;
        }
        while (_value.array->size() < static_cast<std::size_t>(index)) {
            _value.array->push_back(Jsonator());
        }
        _value.array->push_back(Jsonator());
        return _value.array->back();
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
        if (it != _value.array->end()) {
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
        if (it != _value.object->end()) {
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
        if (it != _value.object->end()) {
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
        if (it != _value.array->end()) {
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
        if (it != _value.array->end()) {
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
        bool ret;
        switch (_type) {
            case NONE_TYPE:
            case NUMBER_TYPE:
            case BOOLEAN_TYPE:
                ret = true;
                break;
            case OBJECT_TYPE:
                ret = _value.object->empty();
                break;
            case ARRAY_TYPE:
                ret = _value.array->empty();
                break;
            case STRING_TYPE:
                ret = _value.string->empty();
                break;
        }
        return ret;
    }

    /**
     * @brief reserve placement on vector
     *
     * @param size
     */
    void reserve(std::size_t size) {
        if (isNull()) {
            newArray();
        }
        if (!isArray()) {
            throw AccessException(*this, "is not a array");
        }
        _value.array->reserve(size);
    }

    /**
     * @brief get the const iterator of begin object
     *
     * @return object_const_iterator
     */
    object_const_iterator object_begin() const {
        if (!isObject()) {
            throw AccessException(*this, "is not a object");
        }
        return _value.object->begin();
    }

    /**
     * @brief get the iterator of begin object
     *
     * @return object_const_iterator
     */
    object_iterator object_begin() {
        if (!isObject()) {
            throw AccessException(*this, "is not a object");
        }
        return _value.object->begin();
    }

    /**
     * @brief get the const iterator of begin array
     *
     * @return object_const_iterator
     */
    array_const_iterator array_begin() const {
        if (!isArray()) {
            throw AccessException(*this, "is not a array");
        }
        return _value.array->begin();
    }

    /**
     * @brief get the iterator of begin array
     *
     * @return object_const_iterator
     */
    array_iterator array_begin() {
        if (!isArray()) {
            throw AccessException(*this, "is not a array");
        }
        return _value.array->begin();
    }

    /**
     * @brief get the const iterator of end object
     *
     * @return object_const_iterator
     */
    object_const_iterator object_end() const {
        if (!isObject()) {
            throw AccessException(*this, "is not a object");
        }
        return _value.object->end();
    }

    /**
     * @brief get the iterator of end object
     *
     * @return object_const_iterator
     */
    object_iterator object_end() {
        if (!isObject()) {
            throw AccessException(*this, "is not a object");
        }
        return _value.object->end();
    }

    /**
     * @brief get the const iterator of end array
     *
     * @return object_const_iterator
     */
    array_const_iterator array_end() const {
        if (!isArray()) {
            throw AccessException(*this, "is not a array");
        }
        return _value.array->end();
    }

    /**
     * @brief get the iterator of end array
     *
     * @return object_const_iterator
     */
    array_iterator array_end() {
        if (!isArray()) {
            throw AccessException(*this, "is not a array");
        }
        return _value.array->end();
    }

    /**
     * @brief get the size of object or array or string
     *
     * @return std::size_t
     */
    std::size_t size() const {
        std::size_t ret;
        switch (_type) {
            case NONE_TYPE:
            case NUMBER_TYPE:
            case BOOLEAN_TYPE:
                ret = 0;
                break;
            case OBJECT_TYPE:
                ret = _value.object->size();
                break;
            case ARRAY_TYPE:
                ret = _value.array->size();
                break;
            case STRING_TYPE:
                ret = _value.string->size();
                break;
        }
        return ret;
    }

    /**
     * @brief get first json element
     * create then if not exist
     *
     * @return Jsonator&
     * @throw AccessException if type is not a array
     * @throw ChildException if index is not exists
     */
    Jsonator& front() {
        return operator[](0);
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
        if (isArray()) {
            if (_value.array->size() == 0) {
                return operator[](0);
            }
            else {
                return at(_value.array->size() - 1);
            }
        }
        else {
            return operator[](0);
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
        if (isArray()) {
            if (_value.array->size() == 0) {
                return at(0);
            }
            else {
                return at(_value.array->size() - 1);
            }
        }
        else {
            return at(0);
        }
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
        Jsonator newValue;
        newValue = value;
        if (isArray()) {
            // move elements
            for (std::size_t i = _value.array->size(); i > static_cast<std::size_t>(index); --i) {
                Jsonator& n = operator[](i);
                Jsonator& e = (*_value.array)[i - 1];
                n._swap(e);
            }
        }
        operator[](index)._swap(newValue);
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
        Jsonator newValue;
        newValue = value;
        if (isArray()) {
            // move elements
            for (std::size_t i = _value.array->size(); i > 0; --i) {
                Jsonator& n = operator[](i);
                Jsonator& e = (*_value.array)[i - 1];
                n._swap(e);
            }
        }
        operator[](0)._swap(newValue);
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
        if (isArray()) {
            operator[](_value.array->size()) = value;
        }
        else {
            operator[](0) = value;
        }
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
        if (isArray()) {
            erase(_value.array->size() - 1);
        }
        return *this;
    }

    /**
     * @brief create object
     * @throw AccessException if type is not a null and not a object
     */
    void newObject() {
        if (!isNull() && !isObject()) {
            throw AccessException(*this, "is not a object");
        }
        if (isNull()) {
            _value.object = new std::map<std::string, Jsonator>();
            _type = OBJECT_TYPE;
        }
    }

    /**
     * @brief create array
     * @throw AccessException if type is not a null and not a array
     */
    void newArray() {
        if (!isNull() && !isArray()) {
            throw AccessException(*this, "is not a array");
        }
        if (isNull()) {
            _value.array = new std::vector<Jsonator>();
            _type = ARRAY_TYPE;
        }
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
        if (!isNull() && !isString()) {
            throw AccessException(*this, "is not a string");
        }
        std::ostringstream oss("");
        oss << value;
        if (isNull()) {
            _value.string = new std::string(oss.str());
            _type = STRING_TYPE;
        }
        else {
            *(_value.string) = oss.str();
        }
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
        if (!isNull() && !isNumber()) {
            throw AccessException(*this, "is not a number");
        }
        _value.number = value;
        _type = NUMBER_TYPE;
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
        if (!isNull() && !isBoolean()) {
            throw AccessException(*this, "is not a boolean");
        }
        _value.boolean = value;
        _type = BOOLEAN_TYPE;
    }

    /**
     * @brief create null
     * @throw AccessException if type is not a null
     */
    void newNull() {
        if (!isNull()) {
            throw AccessException(*this, "is not a null");
        }
        _type = NONE_TYPE;
    }

    /**
     * @brief check if type of jsonator is null
     *
     * @return true if type is null else false
     */
    bool isNull() const {
        return _type == NONE_TYPE;
    }

    /**
     * @brief check if type of jsonator is a object
     *
     * @return true if type is a object else false
     */
    bool isObject() const {
        return _type == OBJECT_TYPE;
    }

    /**
     * @brief check if type of jsonator is a array
     *
     * @return true if type is a array else false
     */
    bool isArray() const {
        return _type == ARRAY_TYPE;
    }

    /**
     * @brief check if type of jsonator is a string
     *
     * @return true if type is a string else false
     */
    bool isString() const {
        return _type == STRING_TYPE;
    }

    /**
     * @brief check if type of jsonator is a number
     *
     * @return true if type is a number else false
     */
    bool isNumber() const {
        return _type == NUMBER_TYPE;
    }

    /**
     * @brief check if type of jsonator is a boolean
     *
     * @return true if type is a boolean else false
     */
    bool isBoolean() const {
        return _type == BOOLEAN_TYPE;
    }

    /**
     * @brief check if object contains a @p key
     *
     * @param key
     * @return true if object has key else false
     * @throw AccessException if type is not a object
     */
    bool contains(const std::string& key) const {
        if (!isObject()) {
            throw AccessException(*this, "is not a object");
        }
        return _value.object->find(key) != _value.object->end();
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
        if (!isArray()) {
            throw AccessException(*this, "is not a array");
        }
        return static_cast<std::size_t>(index) < _value.array->size();
    }

    /**
     * @brief serialize jsonator to ostream
     *
     * @param indent
     * @param indentCharacter
     * @return std::string
     */
    void dump(std::ostream& os, std::size_t indent = 0, char indentCharacter = ' ') const;

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
        if (!isObject()) {
            throw AccessException(*this, "is not a object");
        }
        object_iterator it = find(key);
        if (it != _value.object->end()) {
            _value.object->erase(key);
            if (_value.object->empty()) {
                _type = NONE_TYPE;
                delete _value.object;
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
        if (!isArray()) {
            throw AccessException(*this, "is not a array");
        }
        array_iterator it = find(index);
        if (it != _value.array->end()) {
            // move elements
            for (std::size_t i = index; i < _value.array->size() - 1; ++i) {
                Jsonator& e = at(i + 1);
                operator[](i)._swap(e);
            }
            _value.array->erase(_value.array->begin() + (_value.array->size() - 1));
            if (_value.array->empty()) {
                _type = NONE_TYPE;
                delete _value.array;
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
    Jsonator& clear();

    /**
     * @brief get the string of json object
     *
     * @return const std::string&
     * @throw AccessException if json type is not a string
     */
    const std::string& getString() const {
        if (!isString()) {
            throw AccessException(*this, "is not a string");
        }
        return *_value.string;
    }

    /**
     * @brief get the number of json object
     *
     * @return const double&
     * @throw AccessException if json type is not a number
     */
    const double& getNumber() const {
        if (!isNumber()) {
            throw AccessException(*this, "is not a number");
        }
        return _value.number;
    }

    /**
     * @brief get the boolean of json object
     *
     * @return const bool&
     * @throw AccessException if json type is not a boolean
     */
    const bool& getBoolean() const {
        if (!isBoolean()) {
            throw AccessException(*this, "is not a boolean");
        }
        return _value.boolean;
    }

    /**
     * @brief Get the EType object
     *
     * @return const EType&
     */
    const EType& getType() const {
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
        if (isArray()) {
            for (std::size_t i = 0; i < _value.array->size(); ++i) {
                ret.push_back((*_value.array)[i]);
            }
        }
        else if (isObject()) {
            for (std::map<std::string, Jsonator>::const_iterator it = _value.object->begin();
                 it != _value.object->end(); ++it) {
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
        if (isArray()) {
            for (std::size_t i = 0; i < _value.array->size(); ++i) {
                ret.push_back((*_value.array)[i]);
            }
        }
        else if (isObject()) {
            for (std::map<std::string, Jsonator>::const_iterator it = _value.object->begin();
                 it != _value.object->end(); ++it) {
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
        if (isObject()) {
            for (std::map<std::string, Jsonator>::const_iterator it = _value.object->begin();
                 it != _value.object->end(); ++it) {
                ret.insert(std::pair<std::string, T>(it->first, it->second));
            }
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
        if (isArray()) {
            for (std::size_t i = 0; i < _value.array->size(); ++i) {
                ret.insert(std::pair<T, U>(i, (*_value.array)[i]));
            }
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
        if (isArray()) {
            for (std::size_t i = 0; i < _value.array->size(); ++i) {
                ret.push((*_value.array)[i]);
            }
        }
        else if (isObject()) {
            for (std::map<std::string, Jsonator>::const_iterator it = _value.object->begin();
                 it != _value.object->end(); ++it) {
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
        if (isArray()) {
            for (std::size_t i = 0; i < _value.array->size(); ++i) {
                ret.insert((*_value.array)[i]);
            }
        }
        else if (isObject()) {
            for (std::map<std::string, Jsonator>::const_iterator it = _value.object->begin();
                 it != _value.object->end(); ++it) {
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
        if (isArray()) {
            for (std::size_t i = 0; i < _value.array->size(); ++i) {
                ret.push((*_value.array)[i]);
            }
        }
        else if (isObject()) {
            for (std::map<std::string, Jsonator>::const_iterator it = _value.object->begin();
                 it != _value.object->end(); ++it) {
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
        if (isArray()) {
            ret.reserve(_value.array->size());
            for (std::size_t i = 0; i < _value.array->size(); ++i) {
                ret.push_back((*_value.array)[i]);
            }
        }
        else if (isObject()) {
            ret.reserve(_value.object->size());
            for (std::map<std::string, Jsonator>::const_iterator it = _value.object->begin();
                 it != _value.object->end(); ++it) {
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

  private:
    union UValue {
        std::map<std::string, Jsonator>* object;
        std::vector<Jsonator>* array;
        std::string* string;
        double number;
        bool boolean;
    };

    static Jsonator _parseStream(std::istream& stream, const std::string& filename, bool comment, bool additionalNext);

    void _swap(Jsonator& json);

    UValue _value;
    EType _type;
};

} // namespace mblet

#endif // _MBLET_JSONATOR_H_