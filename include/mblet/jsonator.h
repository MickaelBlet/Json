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
            ChildException(const Map& map, const std::string& child) : AccessException(map, "has not a child"),
                _child(child) {
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
            BOOL
        };

        /**
         * @brief overide operator stream for print value
         *
         * @param os
         * @param map
         * @return std::ostream&
         */
        inline friend std::ostream& operator<<(std::ostream& os, const Map& map) {
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
         * @brief overide operator for set value from str
         *
         * @param value : new value
         */
        inline void operator=(const std::string& value) {
            newString(value);
        }

        /**
         * @brief overide operator for set value from str
         *
         * @param value : new value
         */
        inline void operator=(const char* value) {
            newString(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const char& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const unsigned char& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const short& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const unsigned short& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const int& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const unsigned int& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const long& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const unsigned long& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const float& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const double& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for set value from number
         *
         * @param value : new value
         */
        inline void operator=(const long double& value) {
            newNumber(value);
        }

        /**
         * @brief overide operator for copy map
         *
         * @param map
         */
        inline void operator=(const Map& map) {
            this->_key = map._key;
            this->_string = map._string;
            this->_number = map._number;
            this->_bool = map._bool;
            this->_type = map._type;
            std::map<std::string, Map>::operator=(map);
        }

        /**
         * @brief overide operator for get map from index
         *
         * @param index : at index
         * @return Map& : map from index
         */
        inline Map& operator[](unsigned long index) {
            if (_type == NONE) {
                _type = ARRAY;
            }
            if (_type != ARRAY) {
                throw AccessException(*this, "is not a array");
            }
            char str[32];
            ::snprintf(str, sizeof(str), "%lu", index);
            Map::iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            while (size() < index) {
                char strTmp[32];
                ::snprintf(strTmp, sizeof(strTmp), "%lu", size());
                insert(std::pair<std::string, Map>(strTmp, Map(this, strTmp)));
            }
            return insert(std::pair<std::string, Map>(str, Map(this, str))).first->second;
        }

        /**
         * @brief overide operator for get const map from index
         *
         * @param index : at index
         * @return const Map& : map from index
         */
        inline const Map& operator[](unsigned long index) const {
            if (_type != ARRAY) {
                throw AccessException(*this, "is not a array");
            }
            char str[32];
            ::snprintf(str, sizeof(str), "%lu", index);
            Map::const_iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, str);
        }

        /**
         * @brief overide operator for get map from string
         *
         * @param str : at string
         * @return Map& : map from string
         */
        inline Map& operator[](const std::string& str) {
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
        inline const Map& operator[](const std::string& str) const {
            if (_type != OBJECT) {
                throw AccessException(*this, "is not a object");
            }
            Map::const_iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, str);
        }

        inline Map& at(const std::string& str) {
            if (_type != OBJECT) {
                throw AccessException(*this, "is not a object");
            }
            Map::iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, str);
        }

        inline const Map& at(const std::string& str) const {
            if (_type != OBJECT) {
                throw AccessException(*this, "is not a object");
            }
            Map::const_iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, str);
        }

        inline Map& at(const char* str) {
            return at(std::string(str));
        }

        inline const Map& at(const char* str) const {
            return at(std::string(str));
        }

        inline Map& at(unsigned long index) {
            if (_type != ARRAY) {
                throw AccessException(*this, "is not a array");
            }
            char str[32];
            ::snprintf(str, sizeof(str), "%lu", index);
            Map::iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, str);
        }

        inline const Map& at(unsigned long index) const {
            if (_type != ARRAY) {
                throw AccessException(*this, "is not a array");
            }
            char str[32];
            ::snprintf(str, sizeof(str), "%lu", index);
            Map::const_iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(*this, str);
        }

        inline Map& at(long index) {
            return at(static_cast<unsigned long>(index));
        }

        inline const Map& at(long index) const {
            return at(static_cast<unsigned long>(index));
        }

        inline Map& at(unsigned int index) {
            return at(static_cast<unsigned long>(index));
        }

        inline const Map& at(unsigned int index) const {
            return at(static_cast<unsigned long>(index));
        }

        inline Map& at(int index) {
            return at(static_cast<unsigned long>(index));
        }

        inline const Map& at(int index) const {
            return at(static_cast<unsigned long>(index));
        }

        inline Map& at(unsigned short index) {
            return at(static_cast<unsigned long>(index));
        }

        inline const Map& at(unsigned short index) const {
            return at(static_cast<unsigned long>(index));
        }

        inline Map& at(short index) {
            return at(static_cast<unsigned long>(index));
        }

        inline const Map& at(short index) const {
            return at(static_cast<unsigned long>(index));
        }

        inline Map& at(unsigned char index) {
            return at(static_cast<unsigned long>(index));
        }

        inline const Map& at(unsigned char index) const {
            return at(static_cast<unsigned long>(index));
        }

        inline Map& at(char index) {
            return at(static_cast<unsigned long>(index));
        }

        inline const Map& at(char index) const {
            return at(static_cast<unsigned long>(index));
        }

        /**
         * @brief create object
         *
         * @return true : create ok
         * @return false : create ko
         */
        inline bool newObject() {
            if (_type != NONE && _type != OBJECT) {
                return false;
            }
            _string = std::string();
            _number = 0;
            _bool = false;
            _type = OBJECT;
            return true;
        }

        /**
         * @brief create array
         *
         * @return true : create ok
         * @return false : create ko
         */
        inline bool newArray() {
            if (_type != NONE && _type != ARRAY) {
                return false;
            }
            _string = std::string();
            _number = 0;
            _bool = false;
            _type = ARRAY;
            return true;
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
        inline bool newString(const T& value) {
            if (_type != NONE && _type != STRING) {
                return false;
            }
            std::ostringstream oss("");
            oss << value;
            _string = oss.str();
            _number = 0;
            _bool = false;
            _type = STRING;
            return true;
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
        inline bool newNumber(const T& value) {
            if (_type != NONE && _type != NUMBER) {
                return false;
            }
            _string = std::string();
            _number = value;
            _bool = false;
            _type = NUMBER;
            return true;
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
        inline bool newBoolean(const T& value) {
            if (_type != NONE && _type != BOOL) {
                return false;
            }
            _string = std::string();
            _number = 0;
            _bool = value;
            _type = BOOL;
            return true;
        }

        inline bool newNull() {
            if (_type != NONE && _type != NONE) {
                return false;
            }
            _string = std::string();
            _number = 0;
            _bool = false;
            _type = NONE;
            return true;
        }

        inline bool isNull() const {
            return _type == NONE;
        }

        inline bool isObject() const {
            return _type == OBJECT;
        }

        inline bool isArray() const {
            return _type == ARRAY;
        }

        inline bool isString() const {
            return _type == STRING;
        }

        inline bool isNumber() const {
            return _type == NUMBER;
        }

        inline bool isBool() const {
            return _type == BOOL;
        }

        std::string dump(std::size_t indent = 0) const;

        inline Map& erase(const std::string& str) {
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

        inline Map& erase(const char* str) {
            return erase(std::string(str));
        }

        inline Map& erase(unsigned long index) {
            if (_type != ARRAY) {
                throw AccessException(*this, "is not a array");
            }
            char str[32];
            ::snprintf(str, sizeof(str), "%lu", index);
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

        inline Map& erase(long index) {
            return erase(static_cast<unsigned long>(index));
        }

        inline Map& erase(unsigned int index) {
            return erase(static_cast<unsigned long>(index));
        }

        inline Map& erase(int index) {
            return erase(static_cast<unsigned long>(index));
        }

        inline Map& erase(unsigned short index) {
            return erase(static_cast<unsigned long>(index));
        }

        inline Map& erase(short index) {
            return erase(static_cast<unsigned long>(index));
        }

        inline Map& erase(unsigned char index) {
            return erase(static_cast<unsigned long>(index));
        }

        inline Map& erase(char index) {
            return erase(static_cast<unsigned long>(index));
        }

        inline const Map* getParent() const {
            return _parent;
        }

        inline const std::string& getKey() const {
            return _key;
        }

        inline const std::string& getString() const {
            if (_type != STRING) {
                throw AccessException(*this, "is not a string");
            }
            return _string;
        }

        inline const double& getNumber() const {
            if (_type != NUMBER) {
                throw AccessException(*this, "is not a number");
            }
            return _number;
        }

        inline const bool& getBool() const {
            if (_type != BOOL) {
                throw AccessException(*this, "is not a boolean");
            }
            return _bool;
        }

        inline const Type& getType() const {
            return _type;
        }

      private:
        const Map* _parent;
        std::string _key;
        std::string _string;
        double _number;
        bool _bool;
        Type _type;

    };

    /**
     * @brief Parse exception from Exception
     */
    class ParseException : public std::exception {
      public:
        ParseException(const char* filename, const char* message) : std::exception(),
            _filename(filename), _message(message), _line(0), _column(0) {
            std::ostringstream oss("");
            oss << "Parse ";
            if (filename != NULL) {
                oss << filename << ':';
            }
            oss << " (" << message << ").";
            _what = oss.str();
        };
        ParseException(const char* filename, std::size_t line, std::size_t column, const char* message) : std::exception(),
            _filename(filename), _message(message), _line(line), _column(column) {
            std::ostringstream oss("");
            oss << "Parse at ";
            if (filename != NULL) {
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

    inline void setParseComment(bool comment) {
        _parseComment = comment;
    }

    inline void setParseAdditionalNext(bool next) {
        _parseAdditionalNext = next;
    }

    /**
     * @brief read file and load its config
     *
     * @param filename
     * @return true
     * @return false
     */
    void parseFile(const char* filename);

    /**
     * @brief read stream and load its config
     *
     * @param stream
     */
    inline void parseStream(std::istream& stream) {
        _parseStream(stream);
    }

    /**
     * @brief read string and load its config
     *
     * @param str
     */
    inline void parseString(const std::string& str) {
        std::istringstream iss(str);
        _parseStream(iss);
    }

    /**
     * @brief read data and load its config
     *
     * @param data
     * @param size
     */
    inline void parseData(const char* data, std::size_t size) {
        parseString(std::string(data, size));
    }

    /**
     * @brief overide operator for get const map from index
     *
     * @param index : at index
     * @return Map& : map from index
     */
    inline const Map& operator[](std::size_t index) const {
        return _map[index];
    }

    /**
     * @brief overide operator for get const map from string
     *
     * @param str : at str
     * @return Map& : map from string
     */
    inline const Map& operator[](const char* str) const {
        return _map[str];
    }

    /**
     * @brief overide operator for get const map from string
     *
     * @param str : at str
     * @return Map& : map from string
     */
    inline const Map& operator[](const std::string& str) const {
        return _map[str];
    }

    inline const std::string& getFilename() const {
        return _filename;
    }

    inline const Map& getMap() const {
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
    void _parseStream(std::istream& stream);

    std::string _filename;
    bool _parseAdditionalNext;
    bool _parseComment;
    Map _map;
};

} // namespace mblet

#endif // _MBLET_JSONATOR_H_