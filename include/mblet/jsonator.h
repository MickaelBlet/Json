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
#include <string>
#include <vector>

namespace mblet {

class Jsonator {

  public:

    /**
     * @brief Container map by index
     */
    class Map : public std::map<std::string, Map> {
      public:

        /**
         * @brief Basic exception from std::exception
         */
        class AccessException : public std::exception {
          public:
            AccessException() : _str("") {}
            AccessException(const std::string& key, const char* str) {
                std::ostringstream oss("");
                oss << "\"" << key << "\" " << str;
                _str = oss.str();
            }
            virtual ~AccessException() throw() {}
            const char* what() const throw() {
                return _str.c_str();
            }
          protected:
            std::string _str;
        };

        class ChildException : public AccessException {
          public:
            ChildException(const std::string& key, const std::string& child) : AccessException() {
                std::ostringstream oss("");
                oss << "'" << key << "' has not a child '" << child << "'.";
                _str = oss.str();
            }
            virtual ~ChildException() throw() {}
        };

        enum Type {
            UNKNOWN = 0,
            NONE,
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
         * @brief Destroy the Map object
         */
        virtual ~Map();

        /**
         * @brief overide operator for set value from str
         *
         * @param str : new value
         */
        inline void operator=(const char* value) {
            newString(value);
        }

        inline void operator=(const std::string& value) {
            newString(value.c_str());
        }

        inline void operator=(const char& value) {
            newNumber(value);
        }

        inline void operator=(const unsigned char& value) {
            newNumber(value);
        }

        inline void operator=(const short& value) {
            newNumber(value);
        }

        inline void operator=(const unsigned short& value) {
            newNumber(value);
        }

        inline void operator=(const int& value) {
            newNumber(value);
        }

        inline void operator=(const unsigned int& value) {
            newNumber(value);
        }

        inline void operator=(const long& value) {
            newNumber(value);
        }

        inline void operator=(const unsigned long& value) {
            newNumber(value);
        }

        inline void operator=(const float& value) {
            newNumber(value);
        }

        inline void operator=(const double& value) {
            newNumber(value);
        }

        inline void operator=(const long double& value) {
            newNumber(value);
        }

        inline void operator=(const Map& map) {
            this->key = map.key;
            this->str = map.str;
            this->number = map.number;
            this->boolean = map.boolean;
            this->type = map.type;
            std::map<std::string, Map>::operator=(map);
        }

        /**
         * @brief overide operator for get map from index
         *
         * @param index : at index
         * @return Map& : map from index
         */
        inline Map& operator[](unsigned long index) {
            if (type == UNKNOWN) {
                type = ARRAY;
            }
            if (type != ARRAY) {
                throw AccessException(key, "is not a array.");
            }
            char str[32];
            snprintf(str, sizeof(str), "%lu", index);
            Map::iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            while (size() < index) {
                char strTmp[32];
                snprintf(strTmp, sizeof(strTmp), "%lu", size());
                insert(std::pair<std::string, Map>(strTmp, Map()));
            }
            return insert(std::pair<std::string, Map>(str, Map())).first->second;
        }

        /**
         * @brief overide operator for get const map from index
         *
         * @param index : at index
         * @return const Map& : map from index
         */
        inline const Map& operator[](unsigned long index) const {
            if (type != ARRAY) {
                throw AccessException(key, "is not a array.");
            }
            char str[32];
            snprintf(str, sizeof(str), "%lu", index);
            Map::const_iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(key, str);
        }

        /**
         * @brief overide operator for get map from string
         *
         * @param str : at string
         * @return Map& : map from string
         */
        inline Map& operator[](const std::string& str) {
            if (type == UNKNOWN) {
                type = OBJECT;
            }
            if (type != OBJECT) {
                throw AccessException(key, "is not a object.");
            }
            Map::iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            return insert(std::pair<std::string, Map>(str, Map())).first->second;
        }

        /**
         * @brief overide operator for get const map from string
         *
         * @param str : search string
         * @return const Map& : map from string
         */
        inline const Map& operator[](const std::string& str) const {
            if (type != OBJECT) {
                throw AccessException(key, "is not a object.");
            }
            Map::const_iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            throw ChildException(key, str);
        }

        template<typename T>
        bool newString(const T& value) {
            if (type != UNKNOWN && type != STRING) {
                return false;
            }
            std::ostringstream oss("");
            oss << value;
            str = oss.str();
            number = 0;
            boolean = false;
            type = STRING;
            return true;
        }

        template<typename T>
        bool newNumber(const T& value) {
            if (type != UNKNOWN && type != NUMBER) {
                return false;
            }
            std::ostringstream oss("");
            oss << static_cast<double>(value);
            str = oss.str();
            number = value;
            boolean = false;
            type = NUMBER;
            return true;
        }

        template<typename T>
        bool newBoolean(const T& value) {
            if (type != UNKNOWN && type != BOOL) {
                return false;
            }
            std::ostringstream oss("");
            oss << value;
            str = oss.str();
            number = 0;
            boolean = value;
            type = BOOL;
            return true;
        }

        inline bool isUnknown() const {
            return type == UNKNOWN;
        }

        inline bool isNull() const {
            return type == NONE;
        }

        inline bool isObject() const {
            return type == OBJECT;
        }

        inline bool isArray() const {
            return type == ARRAY;
        }

        inline bool isString() const {
            return type == STRING;
        }

        inline bool isNumber() const {
            return type == NUMBER;
        }

        inline bool isBool() const {
            return type == BOOL;
        }

        std::string dump(std::size_t indent = 0) const;

        inline Map& erase(const char* str) {
            if (type != OBJECT) {
                throw AccessException(key, "is not a object.");
            }
            Map::iterator it = find(str);
            if (it != end()) {
                std::map<std::string, Map>::erase(str);
                if (empty()) {
                    type = UNKNOWN;
                }
            }
            else {
                throw AccessException(str, "not found.");
            }
            return *this;
        }

        inline Map& erase(const std::string& str) {
            return erase(str.c_str());
        }

        inline Map& erase(unsigned long index) {
            if (type != ARRAY) {
                throw AccessException(key, "is not a array.");
            }
            char str[32];
            snprintf(str, sizeof(str), "%lu", index);
            Map::iterator it = find(str);
            if (it != end()) {
                std::map<std::string, Map>::erase(str);
                if (empty()) {
                    type = UNKNOWN;
                }
            }
            else {
                throw AccessException(str, "not found.");
            }
            return *this;
        }

        inline Map& erase(int index) {
            return erase(static_cast<unsigned long>(index));
        }

        std::string key;
        std::string str;
        double number;
        bool boolean;
        Type type;

      private:
        /**
         * @brief parse value and use operator << in stream
         *
         * @param stringStream
         */
        void valueToStream(std::ostream& stringStream) const;

    };

    /**
     * @brief Basic exception from std::exception
     */
    class Exception : public std::exception {
      public:
        Exception() : _str(std::string()) {}
        Exception(const char* str) : _str(str) {}
        virtual ~Exception() throw() {}
        const char* what() const throw() {
            return _str.c_str();
        }
      protected:
        std::string _str;
    };

    /**
     * @brief Parse exception from Exception
     */
    class ParseException : public Exception {
      public:
        ParseException(const char* fileName, const char* message) : Exception(""),
            _line(0), _column(0) {
            std::ostringstream oss("");
            oss << "Parse ";
            if (fileName != NULL) {
                oss << fileName << ':';
            }
            oss << " (" << message << ").";
            _str = oss.str();
        };
        ParseException(const char* fileName, std::size_t line, std::size_t column, const char* message) : Exception(""),
            _line(line), _column(column) {
            std::ostringstream oss("");
            oss << "Parse at ";
            if (fileName != NULL) {
                oss << fileName << ':';
            }
            oss << line << ':' << column << " (" << message << ").";
            _str = oss.str();
        };
        virtual ~ParseException() throw() {}
        std::size_t line() const throw() {
            return _line;
        }
        std::size_t column() const throw() {
            return _column;
        }
      protected:
        std::size_t _line;
        std::size_t _column;
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
    inline void parseData(const char* data, std::size_t size) {
        std::istringstream iss(std::string(data, size));
        _parseStream(iss);
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

    inline Map& map() {
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