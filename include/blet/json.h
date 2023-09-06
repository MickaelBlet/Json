/**
 * json.h
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2023 BLET Mickaël.
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
class LoadException : public std::exception {
  public:
    LoadException(const std::string& filename, const std::string& message);
    LoadException(const std::string& filename, std::size_t line, std::size_t column, const std::string& message);
    virtual ~LoadException() throw();
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
 * @brief Parse and load a json from filename.
 *
 * @param filename A filename.
 * @param comment Option for accept the comment (style C/C++) in json.
 * @param additionalNext Option for accept the bad commat at end of json object.
 * @return blet::Dict Dictionnary of json.
 */
blet::Dict loadFile(const char* filename, bool comment = false, bool additionalNext = false);

/**
 * @brief Parse and load a json from stream.
 *
 * @param stream A stream.
 * @param comment Option for accept the comment (style C/C++) in json.
 * @param additionalNext Option for accept the bad commat at end of json object.
 * @return blet::Dict Dictionnary of json.
 */
blet::Dict loadStream(std::istream& stream, bool comment = false, bool additionalNext = false);

/**
 * @brief Parse and load a json from string.
 *
 * @param str A string.
 * @param comment Option for accept the comment (style C/C++) in json.
 * @param additionalNext Option for accept the bad commat at end of json object.
 * @return blet::Dict Dictionnary of json.
 */
blet::Dict loadString(const std::string& str, bool comment = false, bool additionalNext = false);

/**
 * @brief Parse and load a json from data.
 *
 * @param data A data.
 * @param size Size of data.
 * @param comment Option for accept the comment (style C/C++) in json.
 * @param additionalNext Option for accept the bad commat at end of json object.
 * @return blet::Dict Dictionnary of json.
 */
blet::Dict loadData(const void* data, std::size_t size, bool comment = false, bool additionalNext = false);

} // namespace json

} // namespace blet

#endif // #ifndef _BLET_JSON_H_