# Examples

## Parse File
```
$ cat example.json
{
    "string": "foobar",
    "number": 42,
    "boolean": true,
    "null": null,
    "array": [
        1337,
        0.07
    ],
    "object": {
        "key": "value"
    }
}
```
```cpp
const mblet::Jsonator json = mblet::Jsonator::parseFile("example.json");
// get value
std::string str = json["string"];
unsigned int number = json["number"];
bool boolean = json["boolean"];
bool isNull = json["null"].isNull();
std::vector<double> array = json["array"];
std::map<std::string, std::string> map = json["object"];
std::string key = json["object"]["key"];
```