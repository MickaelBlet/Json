#include <iostream>

#include "blet/json.h"

int main(int /*argc*/, char* /*argv*/[]) {
    const blet::Dict json = blet::json::loadFile("./example/example.jsonc", true, true);
    // get value
    std::string str = json["string"];
    unsigned int number = json["number"];
    bool boolean = json["boolean"];
    bool isNull = json["null"].isNull();
    std::vector<double> array = json["array"];
    std::map<std::string, std::string> map = json["object"];
    std::string key = json["object"]["key"];
    // print result
    std::cout << "string:      " << str << '\n'
              << "number:      " << number << '\n'
              << "boolean:     " << boolean << '\n'
              << "null:        " << isNull << '\n'
              << "array[0]:    " << array[0] << '\n'
              << "object[key]: " << map["key"] << '\n'
              << "object[key]: " << key << std::endl;
}
