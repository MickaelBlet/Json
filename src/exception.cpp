/**
 * exception.cpp
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

#include "blet/json.h"

namespace blet {

namespace json {

LoadException::LoadException(const std::string& filename, const std::string& message) :
    std::exception(),
    _filename(filename),
    _message(message),
    _line(0),
    _column(0) {
    std::ostringstream oss("");
    oss << "Parse ";
    if (!_filename.empty()) {
        oss << _filename << ": ";
    }
    oss << '(' << _message << ").";
    _what = oss.str();
}

LoadException::LoadException(const std::string& filename, std::size_t line, std::size_t column,
                             const std::string& message) :
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
}

LoadException::~LoadException() throw() {}

const char* LoadException::what() const throw() {
    return _what.c_str();
}

const std::string& LoadException::filename() const throw() {
    return _filename;
}

const std::string& LoadException::message() const throw() {
    return _message;
}

const std::size_t& LoadException::line() const throw() {
    return _line;
}

const std::size_t& LoadException::column() const throw() {
    return _column;
}

} // namespace json

} // namespace blet