#include <iostream>

#include "blet/json.h"

#define JSON_TO_STRING(x) std::string(#x + 1, sizeof(#x) - 3)

int main(int /*argc*/, char* /*argv*/[]) {
    /*
    ** parse
    */
    const std::string jsonStr = JSON_TO_STRING((
        {
          "hello": "world",
          "array": [
            42,
            [ 1337 ],
            {
              "key_in_array": 0.42
            },
            -42e4
          ],
          "null": null,
          "boolean": false
        }
    ));
    blet::Dict json = blet::json::parseString(jsonStr);
    std::cout << json["array"][0].getNumber() << '\n';
    std::cout << json["array"][1][0].getNumber() << '\n';
    std::cout << json["array"][2]["key_in_array"].getNumber() << '\n';
    std::cout << json["boolean"].getBoolean() << '\n';
    std::cout << json["hello"].getString() << '\n';
    std::cout << json["null"].isNull() << '\n';

    std::cout << json["array"].getArray().size() << std::endl;
    // output:
    // 42
    // 1337
    // 0.42
    // 0
    // world
    // 1

    /*
    ** transform
    */
    std::vector<unsigned int> arraySecond = json["array"][1];
    std::cout << arraySecond[0] << std::endl;
    // output:
    // 1337

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

    blet::Dict newJson;
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

    std::cout << blet::json::dump(newJson, 4) << '\n';
    // output:
    // {
    //     "array": [
    //         "foo",
    //         "bar"
    //     ],
    //     "boolean": true,
    //     "foo": "bar",
    //     "json": {
    //         "array": [
    //             42,
    //             [
    //                 1337
    //             ],
    //             {
    //                 "key_in_array": 0.42
    //             }
    //         ],
    //         "boolean": false,
    //         "hello": "world",
    //         "null": null
    //     },
    //     "map_object": {
    //         "key1": "value1",
    //         "key2": "value2"
    //     },
    //     "null": null,
    //     "number": 24,
    //     "object": {
    //         "foo": "bar"
    //     },
    //     "vector": [
    //         0.42,
    //         -0.42,
    //         42
    //     ]
    // }
    return 0;
}