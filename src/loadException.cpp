/**
 * loadException.cpp
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2024 BLET Mickaël.
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
    filename_(filename),
    message_(message),
    line_(0),
    column_(0) {
    std::ostringstream oss("");
    oss << "Load ";
    if (!filename_.empty()) {
        oss << filename_ << ": ";
    }
    oss << '(' << message_ << ").";
    what_ = oss.str();
}

LoadException::LoadException(const std::string& filename, std::size_t line, std::size_t column,
                             const std::string& message) :
    std::exception(),
    filename_(filename),
    message_(message),
    line_(line),
    column_(column) {
    std::ostringstream oss("");
    oss << "Load at ";
    if (!filename_.empty()) {
        oss << filename_ << ':';
    }
    oss << line_ << ':' << column_ << " (" << message_ << ").";
    what_ = oss.str();
}

LoadException::~LoadException() throw() {}

const char* LoadException::what() const throw() {
    return what_.c_str();
}

const std::string& LoadException::filename() const throw() {
    return filename_;
}

const std::string& LoadException::message() const throw() {
    return message_;
}

const std::size_t& LoadException::line() const throw() {
    return line_;
}

const std::size_t& LoadException::column() const throw() {
    return column_;
}

} // namespace json

} // namespace blet
