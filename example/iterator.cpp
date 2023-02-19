#include <iostream>

#include "mblet/jsonator.h"

int main(int /*argc*/, char* /*argv*/[]) {
    mblet::Jsonator json;
    json["foo"][0] = "0";
    json["foo"][1] = "1";
    json["foo"][2] = "2";
    json["foo"][3].newNull();
    json["foo"][4] = true;
    json["bar"]["0"] = 0;
    json["bar"]["1"] = 1;
    json["bar"]["2"] = 2;
    mblet::Jsonator::array_iterator vit;
    for (vit = json["foo"].array_begin(); vit != json["foo"].array_end(); ++vit) {
        std::cout << "test: " << *vit << std::endl;
    }
    mblet::Jsonator::object_iterator mit;
    for (mit = json["bar"].object_begin(); mit != json["bar"].object_end(); ++mit) {
        std::cout << mit->first << ':' << mit->second << std::endl;
    }
    return 0;
}