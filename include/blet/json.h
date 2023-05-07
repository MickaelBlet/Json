#ifndef _BLET_JSON_H_
#define _BLET_JSON_H_

#include <exception> // std::exception
#include <sstream>   // std::istream, std::ostream, std:ostringstream
#include <string>    // std::string

#include "blet/dict.h"

namespace blet {

namespace json {

/**
 * @brief Parse exception from std::exception
 */
class ParseException : public std::exception {
  public:
    ParseException(const std::string& filename, const std::string& message);
    ParseException(const std::string& filename, std::size_t line, std::size_t column, const std::string& message);
    virtual ~ParseException() throw();
    const char* what() const throw();
    const std::string& filename() const throw();
    const std::string& message() const throw();
    const std::size_t& line() const throw();
    const std::size_t& column() const throw();

  protected:
    std::string _what;
    std::string _filename;
    std::string _message;
    std::size_t _line;
    std::size_t _column;
};

/**
 * @brief dump dict in json format
 *
 * @param dict object for dump
 * @param os output ostream
 * @param indent indentation of dump
 * @param indentCharacter indentation character
 */
void dump(const blet::Dict& dict, std::ostream& os, std::size_t indent = 0, char indentCharacter = ' ');

/**
 * @brief dump dict in json format
 *
 * @param dict object for dump
 * @param indent indentation of dump
 * @param indentCharacter indentation character
 * @return std::string
 */
std::string dump(const blet::Dict& dict, std::size_t indent = 0, char indentCharacter = ' ');

blet::Dict parseFile(const char* filename, bool comment = true, bool additionalNext = true);

blet::Dict parseStream(std::istream& stream, bool comment = true, bool additionalNext = true);

blet::Dict parseString(const std::string& str, bool comment = true, bool additionalNext = true);

blet::Dict parseData(const void* data, std::size_t size, bool comment = true, bool additionalNext = true);

} // namespace json

} // namespace blet

#endif // #ifndef _BLET_JSON_H_