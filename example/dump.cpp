#include <iostream>

#include "blet/json.h"

int main(int /*argc*/, char* /*argv*/[]) {
    blet::Dict json;
    json["foo"] = "bar";
    json["number"] = 42;
    json["null"].newNull();
    json["array"][0] = 4;
    json["array"][1] = 2;
    // print result
    std::cout << blet::json::dump(json) << '\n' << '\n' << blet::json::dump(json, 2) << std::endl;
    return 0;
}
