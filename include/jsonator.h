#ifndef _MBLET_JSONATOR_H_
#define _MBLET_JSONATOR_H_

#include <map>
#include <string>
#include <cstdio>
#include <sstream>
#include <iostream>

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
            AccessException(const std::string& key, const char* str) {
                std::ostringstream oss("");
                oss << "\"" << key << "\" " << str;
                _str = oss.str();
            }
            AccessException(const char* str) : _str(str) {}
            virtual ~AccessException() throw() {}
            const char* what() const throw() {
                return _str.c_str();
            }
        protected:
            std::string _str;
        };

        enum Type {
            NONE = 0,
            OBJECT,
            TABLE,
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
            os << map.str;
            return os;
        }

        /**
         * @brief Construct a new Map object
         */
        Map();

        /**
         * @brief overide operator for get map from index
         *
         * @param index : at index
         * @return Map& : map from index
         */
        // inline Map& operator[](unsigned long index) {
        //     if (type != TABLE) {

        //     }
        //     char str[32];
        //     snprintf(str, sizeof(str), "%lu", index);
        //     return operator[](str);
        // }

        /**
         * @brief overide operator for get const map from index
         *
         * @param index : at index
         * @return const Map& : map from index
         */
        inline const Map& operator[](unsigned long index) const {
            if (type != TABLE) {
                throw AccessException(key, "is not a table.");
            }
            char str[32];
            snprintf(str, sizeof(str), "%lu", index);
            return operator[](str);
        }

        /**
         * @brief overide operator for get map from string
         *
         * @param str : at string
         * @return Map& : map from string
         */
        // inline Map& operator[](const std::string& str) {
        //     Map::iterator it = find(str);
        //     if (it != end()) {
        //         return it->second;
        //     }
        //     insert(std::pair<std::string, Map>(str, Map()));
        //     return at(str);
        // }

        /**
         * @brief overide operator for get const map from string
         *
         * @param str : search string
         * @return const Map& : map from string
         */
        inline const Map& operator[](const std::string& str) const {
            static Map emptyMap;

            Map::const_iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            return emptyMap;
        }

        /**
         * @brief get value
         * convert bibary or octal or hex value
         * bool == "true"||"false", "on"||"off"
         *
         * @tparam T : type of return (from string)
         * @return T : convert value
         */
        template<typename T>
        T get() const {
            T retValue;
            std::stringstream stringStream("");
            valueToStream(stringStream);
            stringStream >> retValue;
            return retValue;
        }

        /**
         * @brief get value with default value
         * convert bibary or octal or hex value
         * bool == "true"||"false", "on"||"off"
         *
         * @tparam T : type of return (from string)
         * @return T : convert value
         */
        template<typename T, typename U>
        T get(const U& defaultValue) const {
            T retValue;
            std::stringstream stringStream("");

            if (str.empty()) {
                stringStream << defaultValue;
            }
            else {
                valueToStream(stringStream);
            }
            stringStream >> retValue;
            return retValue;
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
        ParseException(std::size_t line, std::size_t column, const char* message) : Exception(""), _line(line), _column(column) {
            std::ostringstream oss("");
            oss << "Parse at " << line << ":" << column << " (" << message << ").";
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
     * @brief Destroy the Jsonator object
     */
    ~Jsonator();

    /**
     * @brief Construct a new Jsonator object
     *
     * @param rhs
     */
    Jsonator(const Jsonator& rhs);

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
    bool readFile(const char* filename);

    /**
     * @brief read stream and load its config
     *
     * @param stream
     */
    inline void readStream(std::istream& stream) {
        parseStream(stream);
    }

    /**
     * @brief read string and load its config
     *
     * @param str
     */
    inline void readString(const std::string& str) {
        std::istringstream istr(str);
        parseStream(istr);
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
    inline const Map& operator[](const std::string& str) const {
        return _map[str];
    }

    /**
     * @brief dump in stream
     *
     * @param oss
     * @return std::ostream&
     */
    std::ostream& dump(std::size_t indent = 0, std::ostream& oss = std::cout) const;

private:

    /**
     * @brief parse and fill the map from stream
     *
     * @param stream
     */
    void parseStream(std::istream& stream);

    Map _map;

};

} // namespace mblet

#endif // _MBLET_JSONATOR_H_