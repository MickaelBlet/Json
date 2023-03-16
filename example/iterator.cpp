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
    mblet::Jsonator::array_iterator ait;
    for (ait = json["foo"].array_begin(); ait != json["foo"].array_end(); ++ait) {
        std::cout << "test: " << *ait << std::endl;
    }
    mblet::Jsonator::object_iterator oit;
    for (oit = json["bar"].object_begin(); oit != json["bar"].object_end(); ++oit) {
        std::cout << oit->first << ':' << oit->second << std::endl;
    }
    return 0;
}