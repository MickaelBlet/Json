#include <iostream>

#include "blet/json.h"

int main(int /*argc*/, char* /*argv*/[]) {
    std::string jsonStr("{\"hello\":\"world\"}");
    const blet::Dict json = blet::json::parseString(jsonStr);
    // get value
    std::string str = json["hello"];
    // print result
    std::cout << str << std::endl;
    return 0;
}