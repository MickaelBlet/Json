# Json

Json load and dump library  
Header only library at [single_include/blet/json.h](single_include/blet/json.h).  
Examples at [docs/examples.md](docs/examples.md)

## Quick start

```cpp
/*
** load
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
const blet::Dict json = blet::json::loadString(jsonStr);

std::cout << json["array"][0].getNumber() << '\n';
std::cout << json["array"][1][0].getNumber() << '\n';
std::cout << json["array"][2]["key_in_array"].getNumber() << '\n';
std::cout << json["boolean"].getBoolean() << '\n';
std::cout << json["hello"].getString() << '\n';
std::cout << json["null"].isNull() << '\n';
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
```

## Build

```bash
# Static Release
mkdir build; pushd build; cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=0 .. && make -j && make install; popd
# Dynamic Release
mkdir build; pushd build; cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=1 .. && make -j && make install; popd

# Static Release C++98
mkdir build; pushd build; cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=98 -DBUILD_SHARED_LIBS=0 .. && make -j && make install; popd
# Dynamic Release C++98
mkdir build; pushd build; cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=98 -DBUILD_SHARED_LIBS=1 .. && make -j && make install; popd

# Install with custom directory
mkdir build; pushd build; cmake -DCMAKE_INSTALL_PREFIX="YOUR_INSTALL_PATH" .. && make -j && make install; popd

# Example + Tests + Coverage
mkdir build; pushd build; cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLE=1 -DBUILD_TESTING=1 -DBUILD_COVERAGE=1 -DCMAKE_CXX_STANDARD=98 .. && make -j && make test -j; popd
```

## Load Functions

### loadFile

```cpp
blet::Dict loadFile(const char* filename, bool comment = false, bool additionalNext = false);
```
Take a path of json file and load them for create a Dict object.  
You can enable options `comment` and `additionnalNext` for a better parsing.  
Example at [docs/examples.md#loadFile](docs/examples.md#loadfile).

### loadStream

```cpp
blet::Dict loadStream(std::istream& stream, bool comment = false, bool additionalNext = false);
```

Take a std::istream and load them for create a Dict object.  
You can enable options `comment` and `additionnalNext` for a better parsing.  
Example at [docs/examples.md#loadStream](docs/examples.md#loadstream).

### loadString

```cpp
blet::Dict loadString(const std::string& str, bool comment = false, bool additionalNext = false);
```

Take a std::string and load them for create a Dict object.  
You can enable options `comment` and `additionnalNext` for a better parsing.  
Example at [docs/examples.md#loadString](docs/examples.md#loadstring).

### loadData

```cpp
blet::Dict loadData(const void* data, std::size_t size, bool comment = false, bool additionalNext = false);
```

Take a data and size and load them for create a Dict object.  
You can enable options `comment` and `additionnalNext` for a better parsing.  
Example at [docs/examples.md#loadData](docs/examples.md#loaddata).

## Dump Functions

### Dump

``` cpp
void dump(const blet::Dict& dict, std::ostream& os, std::size_t indent = 0, char indentCharacter = ' ');
```

Dump from a Dict to json format in stream.  
You can set the `indent` and `indentCharacter` for better formating.  
Example at [docs/examples.md#DumpStream](docs/examples.md#dumpstream).

``` cpp
std::string dump(const blet::Dict& dict, std::size_t indent = 0, char indentCharacter = ' ');
```

Dump from a Dict to json format in string.  
You can set the `indent` and `indentCharacter` for better formating.  
Example at [docs/examples.md#DumpStream](docs/examples.md#dumpstring).