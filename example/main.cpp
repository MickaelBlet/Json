#include <vector>

#include "mblet/jsonator.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " JSON_FILE1 JSON_FILE2" << std::endl;
        return 1;
    }
    mblet::Jsonator jsonator;
    jsonator.parseFile(argv[1]);
    std::cout << jsonator.dump(2) << std::endl;
    // mblet::Jsonator::Json json;
    // char* test = (char*)"test";
    // json["key"] = "test";
    // json["type"] = "test";
    // json["array"][0][0][0][0][0][0][0][0][0][0]["test"] = test;
    // jsonator.getJson()["3"] = json;
    // jsonator["object"]["2"];
    std::string strKey2 = jsonator["key"];
    const double& dd = jsonator["number"];
    mblet::Jsonator strKey = jsonator["object"];
    const mblet::Jsonator* ptrMap = &jsonator["key"];
    const char* nu = "number";
    double testd = jsonator[nu].get<double>();
    std::cout << ptrMap->get<std::string>() << std::endl;
    std::cout << jsonator["number"].get<int>() << std::endl;
    std::cout << strKey2 << std::endl;
    std::cout << testd << std::endl;
    std::cout << dd << std::endl;
    std::cout << strKey["null"] << std::endl;
    // std::cout << jsonator.dump(2) << std::endl;
    std::cout << jsonator.dump() << std::endl;
    std::string str = jsonator.dump(0);
    jsonator.parseString(str);
    mblet::Jsonator json(jsonator.getConst());
    std::cout << json.dump(2) << std::endl;
    json["object"]["1"] = "test";
    json["array"][1][1] = 42;
    json[nu] = 32;
    json[nu + 1] = true;
    std::cout << json.dump(2) << std::endl;
    // bool boolean = json[nu + 1];
    json["array\\"][1][1][1][1][1][1] = 42;
    const char ttt[4] = "key";
    json.erase(ttt);
    mblet::Jsonator testai;
    testai.push_back(0);
    testai.push_back("1");
    testai.push_back(2);
    testai.push_back(3);
    std::cout << testai << std::endl;
    testai.push_front(4);
    std::cout << testai << std::endl;
    std::cout << testai[-1] << std::endl;
    testai.erase(3);
    testai.erase(3);
    std::cout << testai << std::endl;
    testai.pop_front();
    testai.pop_front();
    testai.pop_back();
    std::cout << testai << std::endl;
    testai[0];
    std::cout << testai << std::endl;
    mblet::Jsonator testas;
    testas.push_front(42);
    // testas.erase(0)[0];
    json["array"][2] = testai;
    json["array"][3] = testas;
    std::cout << json.at("object").getKey() << std::endl;
    std::cout << json.at("object").isObject() << std::endl;
    std::cout << json.at("object").isArray() << std::endl;
    std::cout << json.at("object").isString() << std::endl;
    std::cout << json.at("array").at(0) << std::endl;
    std::cout << json["array"] << std::endl;
    // std::cout << jsonator["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"]["0"] << std::endl;
    // std::cout << jsonator["0"]["0"]["0"]["0"]["0"]["0"]["0"]["0"].dump(4) <<
    // std::endl; std::cout <<
    // jsonator["0"]["0"]["0"]["0"]["0"]["0"]["0"]["0"]["0"].dump(4) << std::endl;
    // std::cout << json.dump(2) << std::endl;
    // unsigned int u = 0;
    // mblet::Jsonator::Json map2;
    // map2[0][1].erase(u);
    // json[""].erase(0);
    // json[""]["array"][1][0][0].erase(u)[u] = 24;
    json["array"].erase(0).push_front(24);
    std::cout << json["array"] << std::endl;
    json["array"][json["array"].size()].newArray();
    std::cout << json.dump(2) << std::endl;
    json["array"][json["array"].size() - 1].push_back("24");
    std::cout << json.dump(2) << std::endl;
    json["xxx"].parseFile(argv[2]);
    std::cout << json["xxx"].getParent()->dump(2) << std::endl;
    std::cout << json["array"].getFilename() << std::endl;
    return 0;
}