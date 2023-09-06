# Examples

## LoadFile

```jsonc
$ cat ./example/example.jsonc
{
    /*
        multiline
        comment
    */
    "string": "foobar", // inline comment
    "number": 42,
    "boolean": true,
    "null": null,
    "array": [
        1337,
        0.07
    ],
    "object": {
        "key": "value", // extra useless next
    }
}
```

```cpp
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
              << "object[key]: " << key
              << std::endl;
}
```

```
$ ./build/example/loadFile.example
string:      foobar
number:      42
boolean:     1
null:        1
array[0]:    1337
object[key]: value
object[key]: value
```

## LoadStream

```cpp
#include <iostream>

#include "blet/json.h"

int main(int /*argc*/, char* /*argv*/[]) {
    std::istringstream iss("{\"hello\":\"world\"}");
    const blet::Dict json = blet::json::loadStream(iss);
    // get value
    std::string str = json["hello"];
    // print result
    std::cout << str << std::endl;
    return 0;
}
```

```
$ ./build/example/loadStream.example
world
```

## LoadString

```cpp
std::string jsonStr("{\"hello\":\"world\"}");
const blet::Dict json = blet::json::loadString(jsonStr);
// get value
std::string str = json["hello"];
// print result
std::cout << str << std::endl;
```

```
$ ./build/example/loadString.example
world
```

## LoadData

```cpp
std::string jsonStr("{\"hello\":\"world\"}");
const blet::Dict json = blet::json::loadData(jsonStr.c_str(), jsonStr.size());
// get value
std::string str = json["hello"];
// print result
std::cout << str << std::endl;
```

```
$ ./build/example/loadData.example
world
```

## Dump

```cpp
#include "blet/json.h"

int main(int /*argc*/, char* /*argv*/[]) {
    blet::Dict json;
    // construct json
    json["foo"] = "bar";
    json["number"] = 42;
    json["null"].newNull();
    json["array"][0] = 4;
    json["array"][1] = 2;
    // print result
    std::cout << blet::json::dump(json) << '\n' << '\n'
              << blet::json::dump(json, 2) << std::endl;
}
```

```
$ ./build/example/dump.example
{"array":[4,2],"foo":"bar","null":null,"number":42}

{
  "array": [
    4,
    2
  ],
  "foo": "bar",
  "null": null,
  "number": 42
}
```