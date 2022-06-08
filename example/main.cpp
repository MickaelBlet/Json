#include "mblet/jsonator.h"

#include <stdint.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 1;
    }
    mblet::Jsonator jsonator;
    jsonator.parseFile(argv[1]);
    // mblet::Jsonator::Map map;
    // char* test = (char*)"test";
    // map["key"] = "test";
    // map["type"] = "test";
    // map["array"][0][0][0][0][0][0][0][0][0][0]["test"] = test;
    // jsonator.getMap()["3"] = map;
    // jsonator["object"]["2"];
    std::cout << jsonator.dump(2) << std::endl;
    std::cout << jsonator.dump(0) << std::endl;
    std::string str = jsonator.dump(0);
    jsonator.parseString(str);
    mblet::Jsonator::Map map(jsonator.getMap());
    map["object"]["1"] = "test";
    map["array"][1][10] = 42;
    map.erase("key");
    std::cout << map["object"] << std::endl;
    std::cout << map["array"] << std::endl;
    // std::cout << jsonator["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"]["0"]["0"].dump(4) << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"]["0"]["0"]["0"].dump(4) << std::endl;
    std::cout << map.dump(2) << std::endl;
    unsigned int u = 0;
    mblet::Jsonator::Map map2;
    // map2[0][1].erase(u);
    // map[""].erase(0);
    // map[""]["array"][1][0][0].erase(u)[u] = 24;
    map["array"].erase(u)[u] = 24;
    std::cout << map.dump(2) << std::endl;
    return 0;
}