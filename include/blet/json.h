#ifndef _BLET_JSON_H_
#define _BLET_JSON_H_

#include <exception> // std::exception
#include <sstream>   // std::istream, std::ostream
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
 * @brief Dump dict in json format.
 *
 * @param dict A dict.
 * @param os A ostream.
 * @param indent Indentation of dump.
 * @param indentCharacter Indentation character.
 */
void dump(const blet::Dict& dict, std::ostream& os, std::size_t indent = 0, char indentCharacter = ' ');

/**
 * @brief Dump dict in json format.
 *
 * @param dict A dict.
 * @param indent Indentation of dump.
 * @param indentCharacter Indentation character.
 * @return std::string Json string.
 */
std::string dump(const blet::Dict& dict, std::size_t indent = 0, char indentCharacter = ' ');

/**
 * @brief Parse a json from filename.
 *
 * @param filename A filename.
 * @param comment Option for accept the comment (style C/C++) in json.
 * @param additionalNext Option for accept the bad commat at end of json object.
 * @return blet::Dict Dictionnary of json.
 */
blet::Dict parseFile(const char* filename, bool comment = false, bool additionalNext = false);

/**
 * @brief Parse a json from stream.
 *
 * @param stream A stream.
 * @param comment Option for accept the comment (style C/C++) in json.
 * @param additionalNext Option for accept the bad commat at end of json object.
 * @return blet::Dict Dictionnary of json.
 */
blet::Dict parseStream(std::istream& stream, bool comment = false, bool additionalNext = false);

/**
 * @brief Parse a json from string.
 *
 * @param str A string.
 * @param comment Option for accept the comment (style C/C++) in json.
 * @param additionalNext Option for accept the bad commat at end of json object.
 * @return blet::Dict Dictionnary of json.
 */
blet::Dict parseString(const std::string& str, bool comment = false, bool additionalNext = false);

/**
 * @brief Parse a json from data.
 *
 * @param data A data.
 * @param size Size of data.
 * @param comment Option for accept the comment (style C/C++) in json.
 * @param additionalNext Option for accept the bad commat at end of json object.
 * @return blet::Dict Dictionnary of json.
 */
blet::Dict parseData(const void* data, std::size_t size, bool comment = false, bool additionalNext = false);

} // namespace json

} // namespace blet

#endif // #ifndef _BLET_JSON_H_