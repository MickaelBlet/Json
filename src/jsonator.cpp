/**
 * jsonator.cpp
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

#include "mblet/jsonator.h"
#include <fstream>

namespace mblet {

Jsonator::Jsonator() :
    _filename(std::string("")),
    _parseAdditionalNext(true),
    _parseComment(true) {
}

Jsonator::~Jsonator() {
}

Jsonator::Jsonator(const Jsonator& rhs) :
    _filename(rhs._filename),
    _parseAdditionalNext(rhs._parseAdditionalNext),
    _parseComment(rhs._parseComment),
    _map(rhs._map) {
}

Jsonator& Jsonator::operator=(const Jsonator& rhs) {
    if (this == &rhs) {
        return *this;
    }
    _filename = rhs._filename;
    _parseAdditionalNext = rhs._parseAdditionalNext;
    _parseComment = rhs._parseComment;
    _map = rhs._map;
    return *this;
}

void Jsonator::parseFile(const char* filename) {
    _filename = "";

    std::ifstream fileStream(filename); // open file
    if (fileStream.is_open()) {
        _filename = filename;
        _parseStream(fileStream); // parse file
        fileStream.close();
    }
    else {
        throw ParseException(filename, "Open file failed");
    }
}

} // namespace mblet