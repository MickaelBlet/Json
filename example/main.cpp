#include <iostream>

#include "mblet/jsonator.h"

int main(int /*argc*/, char* /*argv*/[]) {
    /*
    ** parse
    */
    const char jsonStr[] =
        "{"
        "  \"hello\": \"world\","
        "  \"array\": ["
        "    42,"
        "    [ 1337 ],"
        "    {"
        "      \"key_in_array\": 0.42"
        "    }"
        "  ],"
        "  \"null\": null,"
        "  \"boolean\": false"
        "}";
    const mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
    std::cout << json["array"][0].getNumber() << '\n';                 // 42
    std::cout << json["array"][1][0].getNumber() << '\n';              // 1337
    std::cout << json["array"][2]["key_in_array"].getNumber() << '\n'; // 0.42
    std::cout << json["boolean"].getBoolean() << '\n';                 // 0
    std::cout << json["hello"].getString() << '\n';                    // world
    std::cout << json["null"].isNull() << '\n';                        // 1

    /*
    ** generate
    */
    std::vector<double> vDouble;
    vDouble.reserve(3);
    vDouble.push_back(0.42);
    vDouble.push_back(-0.42);
    vDouble.push_back(42);

    std::map<std::string, std::string> mStr;
    mStr["key1"] = "value1";
    mStr["key2"] = "value2";

    mblet::Jsonator newJson;
    newJson["foo"] = "bar";
    newJson["array"][0] = "foo";
    newJson["array"][1] = "bar";
    newJson["vector"] = vDouble;
    newJson["object"]["foo"] = "bar";
    newJson["map_object"] = mStr;
    newJson["boolean"] = true;
    newJson["number"] = 24;
    newJson["null"].newNull();
    newJson["json"] = json;

    std::cout << newJson.dump(4) << '\n';
    return 0;
}