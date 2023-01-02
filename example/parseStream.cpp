#include <iostream>

#include "mblet/jsonator.h"

int main(int /*argc*/, char* /*argv*/[]) {
    std::istringstream iss("{\"hello\":\"world\"}");
    const mblet::Jsonator json = mblet::Jsonator::parseStream(iss);
    // get value
    std::string str = json["hello"];
    // print result
    std::cout << str << std::endl;
    return 0;
}