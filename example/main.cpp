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
    // jsonator.getMap()["2"] = map;
    std::cout << jsonator.dump(2) << std::endl;
    std::cout << jsonator.dump(0) << std::endl;
    std::string str = jsonator.dump(0);
    jsonator.parseString(str);
    jsonator.map()["0"]["1"] = "test";
    jsonator.map()["1"][0][0] = 42;
    jsonator.map()["0"].erase("0");
    std::cout << jsonator["0"] << std::endl;
    std::cout << jsonator["1"] << std::endl;
    // std::cout << jsonator["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"]["0"]["0"].dump(4) << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"]["0"]["0"]["0"].dump(4) << std::endl;
    std::cout << jsonator.dump(2) << std::endl;
    jsonator.map()["1"][0].erase(0)[0] = 24;
    std::cout << jsonator.dump(2) << std::endl;
    return 0;
}