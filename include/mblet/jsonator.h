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
class Jsonator {

  public:

    /**
     * @brief multi level of map
     */
    class Map : public std::map<std::string, Map> {
      public:

        /**
         * @brief Access exception from std::exception
         */
        class AccessException : public std::exception {
          public:
            AccessException(const Map& map, const char* message) : std::exception(), _message(message), _map(map) {
                std::stack<const Map*> maps;
                const Map* pMap = map._parent;
                while (pMap != NULL) {
                    maps.push(pMap);
                    pMap = pMap->_parent;
                }
                std::ostringstream oss("");
                while (!maps.empty()) {
                    if (maps.top()->_parent && maps.top()->_parent->isObject()) {
                        oss << '\"' << maps.top()->_key << '\"' << ':';
                    }
                    if (maps.top()->_parent && maps.top()->_parent->isArray()) {
                        oss << '[' << maps.top()->_key << ']' << ':';
                    }
                    maps.pop();
                }
                if (map._parent && map._parent->isObject()) {
                    oss << '\"' << map._key << '\"';
                }
                if (map._parent && map._parent->isArray()) {
                    oss << '[' << map._key << ']';
                }
                oss << ' ' << message << '.';
                _what = oss.str();
            }
            virtual ~AccessException() throw() {}
            const char* what() const throw() {
                return _what.c_str();
            }
            const std::string& message() const throw() {
                return _message;
            }
            const Map& map() const throw() {
                return _map;
            }
          protected:
            std::string _what;
            const std::string _message;
            const Map& _map;
        };

        class ChildException : public AccessException {
          public:
            ChildException(const Map& map, unsigned long index) : AccessException(map, "has not a child") {
                char str[32]; \
                ::snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(index));
                generateWhat(map, str);
            }
            ChildException(const Map& map, const std::string& child) : AccessException(map, "has not a child"),
                _child(child) {
                generateWhat(map, child);
            }
            virtual ~ChildException() throw() {}
            const std::string& child() const throw() {
                return _child;
            }
          protected:
            const std::string _child;
          private:
            void generateWhat(const Map& map, const std::string& child) {
                std::stack<const Map*> maps;
                const Map* pMap = map._parent;
                while (pMap != NULL) {
                    maps.push(pMap);
                    pMap = pMap->_parent;
                }
                std::ostringstream oss("");
                while (!maps.empty()) {
                    if (maps.top()->_parent && maps.top()->_parent->isObject()) {
                        oss << '\"' << maps.top()->_key << '\"' << ':';
                    }
                    if (maps.top()->_parent && maps.top()->_parent->isArray()) {
                        oss << '[' << maps.top()->_key << ']' << ':';
                    }
                    maps.pop();
                }
                if (map._parent && map._parent->isObject()) {
                    oss << '\"' << map._key << '\"';
                }
                if (map._parent && map._parent->isArray()) {
                    oss << '[' << map._key << ']';
                }
                oss << " has not a child '" << child << "'.";
                _what = oss.str();
            }
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
            }
        }

        /**
         * @brief overide operator stream for print value
         *
         * @param os
         * @param map
         * @return std::ostream&
         */
        friend std::ostream& operator<<(std::ostream& os, const Map& map) {
            os << map.dump();
            return os;
        }

        /**
         * @brief Construct the Map object
         */
        Map();

        /**
         * @brief Construct a new Map object
         *
         * @param key
         */
        Map(const Map* parent, const std::string& key);

        /**
         * @brief Construct a new Map object
         *
         * @param rhs
         */
        Map(const Map& rhs);

        /**
         * @brief Destroy the Map object
         */
        virtual ~Map();

        /**
         * @brief overide operator for copy map
         *
         * @param map
         */
        void operator=(const Map& map) {
            _key = map._key;
            _string = map._string;
            _number = map._number;
            _boolean = map._boolean;
            _type = map._type;
            std::map<std::string, Map>::operator=(map);
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
            newString(value);
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

        Map::iterator find(const std::string& str) {
            return std::map<std::string, Map>::find(str);
        }

        Map::const_iterator find(const std::string& str) const {
            return std::map<std::string, Map>::find(str);
        }

        Map::iterator find(const char* str) {
            return std::map<std::string, Map>::find(str);
        }

        Map::const_iterator find(const char* str) const {
            return std::map<std::string, Map>::find(str);
        }

        template<size_t Size>
        Map::iterator find(const char (&str)[Size]) {
            return std::map<std::string, Map>::find(str);
        }

        template<size_t Size>
        Map::const_iterator find(const char (&str)[Size]) const {
            return std::map<std::string, Map>::find(str);
        }

        template<typename T>
        Map::iterator find(const T& index) {
            char str[32]; \
            ::snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(index));
            return std::map<std::string, Map>::find(str);
        }

        template<typename T>
        Map::const_iterator find(const T& index) const {
            char str[32]; \
            ::snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(index));
            return std::map<std::string, Map>::find(str);
        }

        /**
         * @brief overide operator for get map from string
         *
         * @param str : at string
         * @return Map& : map from string
         */
        Map& operator[](const std::string& str) {
            if (_type == NONE) {
                _type = OBJECT;
            }
            if (_type != OBJECT) {
                throw AccessException(*this, "is not a object");
            }
            Map::iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            return insert(std::pair<std::string, Map>(str, Map(this, str))).first->second;
        }

        /**
         * @brief overide operator for get const map from string
         *
         * @param str : search string
         * @return const Map& : map from string
         */
        const Map& operator[](const std::string& str) const {
            if (_type != OBJECT) {
                throw AccessException(*this, "is not a object");
            }
            Map::const_iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, str);
        }

        Map& operator[](const char* str) {
            return operator[](std::string(str));
        }

        const Map& operator[](const char* str) const {
            return operator[](std::string(str));
        }

        template<size_t Size>
        Map& operator[](const char (&str)[Size]) {
            return operator[](std::string(str));
        }

        template<size_t Size>
        const Map& operator[](const char (&str)[Size]) const {
            return operator[](std::string(str));
        }

        template<typename T>
        Map& operator[](const T& index) {
            if (_type == NONE) {
                _type = ARRAY;
            }
            if (_type != ARRAY) {
                throw AccessException(*this, "is not a array");
            }
            char str[32];
            ::snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(index));
            Map::iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            while (static_cast<unsigned long>(size()) < static_cast<unsigned long>(index)) {
                char strIndex[32]; \
                ::snprintf(strIndex, sizeof(strIndex), "%lu", static_cast<unsigned long>(size()));
                insert(std::pair<std::string, Map>(strIndex, Map(this, strIndex)));
            }
            return insert(std::pair<std::string, Map>(str, Map(this, str))).first->second;
        }

        template<typename T>
        const Map& operator[](const T& index) const {
            if (_type != ARRAY) {
                throw AccessException(*this, "is not a array");
            }
            Map::const_iterator it = find(index);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, index);
        }

        Map& at(const std::string& str) {
            if (_type != OBJECT) {
                throw AccessException(*this, "is not a object");
            }
            Map::iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, str);
        }

        const Map& at(const std::string& str) const {
            if (_type != OBJECT) {
                throw AccessException(*this, "is not a object");
            }
            Map::const_iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, str);
        }

        Map& at(const char* str) {
            return at(std::string(str));
        }

        const Map& at(const char* str) const {
            return at(std::string(str));
        }

        template<size_t Size>
        Map& at(const char (&str)[Size]) {
            return at(std::string(str));
        }

        template<size_t Size>
        const Map& at(const char (&str)[Size]) const {
            return at(std::string(str));
        }

        template<typename T>
        Map& at(const T& index) {
            if (_type != ARRAY) {
                throw AccessException(*this, "is not a array");
            }
            Map::iterator it = find(index);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, index);
        }

        template<typename T>
        const Map& at(const T& index) const {
            if (_type != ARRAY) {
                throw AccessException(*this, "is not a array");
            }
            Map::const_iterator it = find(index);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, index);
        }

        template<typename T>
        void push_front(const T& value) {
            for (std::size_t i = size() ; i > 0 ; --i) {
                Map& m = at(i - 1);
                char str[32];
                ::snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(i));
                m._key = str;
                operator[](i) = m;
            }
            {
                // create and replace first value
                Map m(this, "0");
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
         *
         * @return true : create ok
         * @return false : create ko
         */
        void newObject() {
            if (_type != NONE && _type != OBJECT) {
                throw AccessException(*this, "is not a object");
            }
            _type = OBJECT;
        }

        /**
         * @brief create array
         *
         * @return true : create ok
         * @return false : create ko
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
         * @return true : create ok
         * @return false : create ko
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
         * @return true : create ok
         * @return false : create ko
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
         * @return true : create ok
         * @return false : create ko
         */
        template<typename T>
        void newBoolean(const T& value) {
            if (_type != NONE && _type != BOOLEAN) {
                throw AccessException(*this, "is not a boolean");
            }
            _boolean = value;
            _type = BOOLEAN;
        }

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

        Map& erase(const std::string& str) {
            if (_type != OBJECT) {
                throw AccessException(*this, "is not a object");
            }
            Map::iterator it = find(str);
            if (it != end()) {
                std::map<std::string, Map>::erase(str);
                if (empty()) {
                    _type = NONE;
                }
            }
            else {
                throw ChildException(*this, str);
            }
            return *this;
        }

        Map& erase(const char* str) {
            return erase(std::string(str));
        }

        template<size_t Size>
        Map& erase(const char (&str)[Size]) {
            return erase(std::string(str));
        }

        template<typename T>
        Map& erase(const T& index) {
            if (_type != ARRAY) {
                throw AccessException(*this, "is not a array");
            }
            Map::iterator it = find(index);
            if (it != end()) {
                for (std::size_t i = index ; i < size() - 1; ++i) {
                    Map& m = at(i + 1);
                    char str[32];
                    ::snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(i));
                    m._key = str;
                    operator[](i) = m;
                }
                {
                    char str[32];
                    ::snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(size() - 1));
                    std::map<std::string, Map>::erase(str);
                }
                if (empty()) {
                    _type = NONE;
                }
            }
            else {
                throw ChildException(*this, index);
            }
            return *this;
        }

        const Map* getParent() const {
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

      private:
        const Map* _parent;
        std::string _key;
        std::string _string;
        double _number;
        bool _boolean;
        Type _type;

    };

    /**
     * @brief Parse exception from Exception
     */
    class ParseException : public std::exception {
      public:
        ParseException(const std::string& filename, const std::string& message) : std::exception(),
            _filename(filename), _message(message), _line(0), _column(0) {
            std::ostringstream oss("");
            oss << "Parse ";
            if (!filename.empty()) {
                oss << filename << ':';
            }
            oss << " (" << message << ").";
            _what = oss.str();
        };
        ParseException(const std::string& filename, std::size_t line, std::size_t column, const std::string& message) : std::exception(),
            _filename(filename), _message(message), _line(line), _column(column) {
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
     * @brief Construct a new Jsonator object
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
     * @brief Operator = overide
     *
     * @param rhs
     * @return Jsonator&
     */
    Jsonator& operator=(const Jsonator& rhs);

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
        _parseStream(iss, comment, additionalNext);
    }

    /**
     * @brief read data and load its config
     *
     * @param data
     * @param size
     */
    void parseData(const char* data, std::size_t size, bool comment = true, bool additionalNext = true) {
        parseString(std::string(data, size), comment, additionalNext);
    }

    /**
     * @brief overide operator for get const map from key
     *
     * @param index : at index
     * @return Map& : map from index
     */
    template<typename T>
    const Map& operator[](const T& key) const {
        return _map.at(key);
    }

    const std::string& getFilename() const {
        return _filename;
    }

    const Map& getMap() const {
        return _map;
    }

    /**
     * @brief dump map object
     *
     * @param oss
     * @return std::ostream&
     */
    std::string dump(std::size_t indent = 0) const {
        return _map.dump(indent);
    }

  private:

    /**
     * @brief parse and fill the map from stream
     *
     * @param stream
     */
    void _parseStream(std::istream& stream, bool comment, bool additionalNext);

    std::string _filename;
    Map _map;
};

} // namespace mblet

#endif // _MBLET_JSONATOR_H_