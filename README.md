# Jsonator
Json parse and dump library

## Example

```jsonc
{
    /* comment */
    "key": "string",
    "array": [
        0
    ],
    "number": 0,
    "object": {
        "null": null
    },
    "bool": false
}
```

```cpp
#include "mblet/jsonator.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " JSON_FILE" << std::endl;
        return 1;
    }

    mblet::Jsonator jsonator;

    jsonator.parseFile(argv[1]);
    std::string key = jsonator["key"].getString();
    std::cout << jsonator.dump(2) << std::endl;
    // {
    //   "array": [
    //     0
    //   ],
    //   "bool": false,
    //   "key": "string",
    //   "number": 0,
    //   "object": {
    //     "null": null
    //   }
    // }
    std::cout << jsonator.dump() << std::endl;
    // {"array":[0],"bool":false,"key":"string","number":0,"object":{"null":null}}
    mblet::Jsonator::Map map(jsonator.getMap()); // copy of map
    map["object"]["1"] = "test";
    map["array"][1][1] = 42; // add extra null at 0 position
    map.erase("key");
    std::cout << map.dump(2) << std::endl;
    // {
    //   "array": [
    //     0,
    //     [
    //       null,
    //       42
    //     ]
    //   ],
    //   "bool": false,
    //   "number": 0,
    //   "object": {
    //     "null": null,
    //     "1": "test"
    //   }
    // }
    return 0;
}
```

## Build

```bash
# Static
mkdir build; pushd build; cmake .. && make -j && make install; popd
# Dynamic
mkdir build; pushd build; cmake -DBUILD_SHARED_LIBS=1 .. && make -j && make install; popd
# Static C++98
mkdir build; pushd build; cmake -DCMAKE_CXX_STANDARD=98 .. && make -j && make install; popd
# Example + Tests + Coverage
mkdir build; pushd build; cmake -DBUILD_EXAMPLE=1 -DBUILD_TESTING=1 -DBUILD_COVERAGE=1 -DCMAKE_CXX_STANDARD=98 .. && make -j && make test; popd
# Install with custom directory
mkdir build; pushd build; cmake -DCMAKE_INSTALL_PREFIX="YOUR_INSTALL_PATH" .. && make -j && make install; popd
```