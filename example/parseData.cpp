#include <iostream>

#include "mblet/jsonator.h"

int main(int /*argc*/, char* /*argv*/[]) {
    std::string jsonStr("{\"hello\":\"world\"}");
    const mblet::Jsonator json = mblet::Jsonator::parseData(jsonStr.c_str(), jsonStr.size());
    // get value
    std::string str = json["hello"];
    // print result
    std::cout << str << std::endl;
    return 0;
}