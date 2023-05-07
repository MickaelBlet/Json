#include <iostream>

#include "blet/json.h"

int main(int /*argc*/, char* /*argv*/[]) {
    std::istringstream iss("{\"hello\":\"world\"}");
    const blet::Dict json = blet::json::parseStream(iss);
    // get value
    std::string str = json["hello"];
    // print result
    std::cout << str << std::endl;
    return 0;
}