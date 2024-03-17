#include <iostream>

#include "blet/json.h"

int main(int /*argc*/, char* /*argv*/[]) {
    std::string jsonStr("{/*comment*/\"hello\":\"world\"}");
    blet::Dict json = blet::json::loadData(jsonStr.c_str(), jsonStr.size(), true);
    // get value
    std::string str = json["hello"];
    // print result
    std::cout << str << std::endl;
    return 0;
}
