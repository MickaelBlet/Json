#include <iostream>

#include "mblet/jsonator.h"

int main(int /*argc*/, char* /*argv*/[]) {
    mblet::Jsonator json;
    json["foo"] = "bar";
    json["number"] = 42;
    json["null"].newNull();
    json["array"][0] = 4;
    json["array"][1] = 2;
    // print result
    std::cout << json.dump() << '\n' << '\n' << json.dump(2) << std::endl;
    return 0;
}