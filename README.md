# Jsonator

Json parse and dump library  
Examples at [docs/examples.md](docs/examples.md)

## Quick start

```cpp
/*
** parse
*/
const char jsonStr[] = \
    "{"                             \
    "  \"hello\": \"world\","       \
    "  \"array\": ["                \
    "    42,"                       \
    "    [ 1337 ],"                 \
    "    {"                         \
    "      \"key_in_array\": 0.42"  \
    "    }"                         \
    "  ],"                          \
    "  \"null\": null,"             \
    "  \"boolean\": false"          \
    "}";
const mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
std::cout << json["array"][0].getNumber() << '\n';                  // 42
std::cout << json["array"][1][0].getNumber() << '\n';               // 1337
std::cout << json["array"][2]["key_in_array"].getNumber() << '\n';  // 0.42
std::cout << json["boolean"].getBoolean() << '\n';                  // 0
std::cout << json["hello"].getString() << '\n';                     // world
std::cout << json["null"].isNull() << '\n';                         // 1

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

## Parse Methods

### parseFile

```cpp
static Jsonator parseFile(const char* filename, bool comment = true, bool additionalNext = true);
```
Take a path of json file and parse them for create Jsonator object.  
You can disable options `comment` and `additionnalNext` for a better parsing.  
Example at [docs/examples.md#ParseFile](docs/examples.md#parsefile).

### parseStream

```cpp
static Jsonator parseStream(std::istream& stream, bool comment = true, bool additionalNext = true);
```

Take a std::istream and parse them  for create Jsonator object.  
You can disable options `comment` and `additionnalNext` for a better parsing.  
Example at [docs/examples.md#ParseStream](docs/examples.md#parsestream).

### parseString

```cpp
static Jsonator parseString(const std::string& str, bool comment = true, bool additionalNext = true);
```

Take a std::string and parse them  for create Jsonator object.  
You can disable options `comment` and `additionnalNext` for a better parsing.  
Example at [docs/examples.md#ParseString](docs/examples.md#parsestring).

### parseData

```cpp
static Jsonator parseData(const void* data, std::size_t size, bool comment = true, bool additionalNext = true);
```

Take a data and size and parse them  for create Jsonator object.  
You can disable options `comment` and `additionnalNext` for a better parsing.  
Example at [docs/examples.md#ParseData](docs/examples.md#parsedata).

## Setter Methods

### Operator[] and Operator=

Use bracket and equal operator.  

```cpp
json["string"] = "value";
json["boolean"] = true;
json["number"] = 42;
json["array"][0] = "array0";
json["array"][1] = "array1";
/* ------------------------------------------
{
    "array": [
        "array0",
        "array1"
    ],
    "boolean": true,
    "number": 42,
    "string": "value"
}
------------------------------------------ */
```

### New*

Use the new methods.  

```cpp
json.newObject();
json["string"].newString("value");
json["boolean"].newBoolean(true);
json["number"].newNumber(42);
json["array"].newArray();
json["array"][0].newString("array0");
json["array"][1].newNumber(42);
json["array"][2].newObject();
json["array"][2]["null"].newNull();
/* ------------------------------------------
{
    "array": [
        "array0",
        42,
        {
            "null": null
        }
    ],
    "boolean": true,
    "number": 42,
    "string": "value"
}
------------------------------------------ */
```

### From std container

`deque`, `list`, `map`, `queue`, `set`, `stack`, `vector`

```cpp
std::vector<double> vDouble;
vDouble.reserve(3);
vDouble.push_back(0.42);
vDouble.push_back(-0.42);
vDouble.push_back(42);

std::map<std::string, std::string> mStr;
mStr["key1"] = "value1";
mStr["key2"] = "value2";

json["vector"] = vDouble;
json["map"] = mStr;
/* ------------------------------------------
{
    "map": {
        "key1": "value1",
        "key2": "value2"
    },
    "vector": [
        0.42,
        -0.42,
        42
    ]
}
------------------------------------------ */
```

## Getter Methods

### Dump

Convert Jsonator object to string.  
Example at [docs/examples.md#Dump](docs/examples.md#dump).

### GetFilename

Get filename if you used parseFile method.  
Example at [docs/examples.md#ParseFile](docs/examples.md#dump).

```cpp
const mblet::Jsonator json = mblet::Jsonator::parseFile("./example.json");
std::cout << json.getFilename() << std::endl;
```

### Contains

Check if json object has key.  

```cpp
json["string"] = "value";
json["boolean"] = true;
json["number"] = 42;

std::cout << json.contains("string") << '\n'  // 1
          << json.contains("boolean") << '\n' // 1
          << json.contains("number") << '\n'  // 1
          << json.contains("foo")             // 0
          << std::endl;
```

### Get*

Get type of value.  

`getString`, `getBoolean`, `getNumber`, `getParent`, `getKey`, `getType`, `get`

```cpp
json["string"] = "value";
json["boolean"] = true;
json["number"] = 42;
json["array"][0] = "array0";
json["array"][1] = "array1";

std::cout << json["string"].getString() << '\n'   // value
          << json["boolean"].getBoolean() << '\n' // true
          << json["number"].getNumber() << '\n'   // 42
          << json["array"][1].getString()         // array1
          << std::endl;
```

### Is*

Check type of json object.  

`isNull`, `isObject`, `isArray`, `isString`, `isNumber`, `isBoolean`

```cpp
json["string"].newString("value");
json["boolean"].newBoolean(true);
json["number"].newNumber(42);
json["array"][0].newString("array0");
json["array"][1].newNumber(42);
json["array"][2].newObject();
json["array"][2]["null"].newNull();

std::cout << json["string"].isString() << '\n'         // 1
          << json["boolean"].isBoolean() << '\n'       // 1
          << json["number"].isNumber() << '\n'         // 1
          << json["array"].isArray() << '\n'           // 1
          << json["array"][2].isObject() << '\n'       // 1
          << json["array"][2]["null"].isNull() << '\n' // 1
          << std::endl;
```