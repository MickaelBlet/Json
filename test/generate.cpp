#include <gtest/gtest.h>

#include "mblet/jsonator.h"

#define JSON_TO_STRING(...) #__VA_ARGS__

static std::string removeSpace(const std::string& str) {
    std::string ret("");
    for (std::size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '"') {
            ret.push_back(str[i]);
            ++i;
            while (str[i] != '\n' && str[i] != '\0' && str[i] != '"') {
                if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
                    ret.push_back(str[i]);
                    ++i; // escape character
                }
                ret.push_back(str[i]);
                ++i;
            }
        }
        if (!::isspace(str[i])) {
            ret.push_back(str[i]);
        }
    }
    return ret;
}

GTEST_TEST(jsonator, not_null) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json[0] = json;
                json[0] = json;
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "[0] is not null (is ARRAY).");
                EXPECT_EQ(e.message(), "is not null");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
}

GTEST_TEST(jsonator, equal_operator) {
    // clang-format off
    const char jsonStr[] = JSON_TO_STRING(
        {
            "arrayChar": "string",
            "boolean": true,
            "c": 42,
            "d": 42,
            "deque": [ 1337, 42 ],
            "f": 42,
            "i": 42,
            "jsonator": null,
            "l": 42,
            "list": [ 1337, 42 ],
            "map": { "bar": 42, "foo": 1337 },
            "queue": [ 1337, 42 ],
            "s": 42,
            "set": [ 42, 1337 ],
            "stack": [ 42, 1337 ],
            "starChar": "string",
            "string": "string",
            "uc": 42,
            "ui": 42,
            "ul": 42,
            "us": 42,
            "vector": [ 1337, 42 ]
        }
    );
    // clang-format on
    const std::string jsonStrZip(removeSpace(jsonStr));

    mblet::Jsonator jsonator;

    std::string string = "string";
    const char* starChar = "string";
    const char arrayChar[] = "string";

    bool boolean = true;

    std::deque<int> deque(2);
    deque[0] = 1337;
    deque[1] = 42;
    std::list<int> list;
    list.push_back(1337);
    list.push_back(42);
    std::map<std::string, int> map;
    map["foo"] = 1337;
    map["bar"] = 42;
    std::queue<int> queue;
    queue.push(1337);
    queue.push(42);
    std::set<int> set;
    set.insert(1337);
    set.insert(42);
    std::stack<int> stack;
    stack.push(1337);
    stack.push(42);
    std::vector<int> vector;
    vector.push_back(1337);
    vector.push_back(42);

    char c = 42;
    unsigned char uc = 42;
    short s = 42;
    unsigned short us = 42;
    int i = 42;
    unsigned int ui = 42;
    long l = 42;
    unsigned long ul = 42;
    float f = 42;
    double d = 42;

    mblet::Jsonator json;
    json["jsonator"] = jsonator;
    json["string"] = string;
    json["starChar"] = starChar;
    json["arrayChar"] = arrayChar;
    json["boolean"] = boolean;
    json["deque"] = deque;
    json["list"] = list;
    json["map"] = map;
    json["queue"] = queue;
    json["set"] = set;
    json["stack"] = stack;
    json["vector"] = vector;
    json["c"] = c;
    json["uc"] = uc;
    json["s"] = s;
    json["us"] = us;
    json["i"] = i;
    json["ui"] = ui;
    json["l"] = l;
    json["ul"] = ul;
    json["f"] = f;
    json["d"] = d;

    EXPECT_EQ(jsonStrZip, json.dump());
}

GTEST_TEST(jsonator, generate) {
    // clang-format off
    const char jsonStr[] = JSON_TO_STRING(
        {
            "root" : {
                "1": [
                    0,
                    1,
                    2
                ],
                "2": true,
                "3": null,
                "4": 42,
                "5": {
                    "0": 0,
                    "1": 1
                },
                "6": "42"
            }
        }
    );
    // clang-format on
    const std::string jsonStrZip(removeSpace(jsonStr));

    {
        mblet::Jsonator json;
        json["root"]["1"].newArray();
        json["root"]["1"][0].newNumber(0);
        json["root"]["1"][1].newNumber(1);
        json["root"]["1"][2].newNumber(2);
        json["root"]["2"].newBoolean(true);
        json["root"]["3"].newNull();
        json["root"]["4"].newNumber(42);
        json["root"]["5"].newObject();
        json["root"]["5"]["0"].newNumber(0);
        json["root"]["5"]["1"].newNumber(1);
        json["root"]["6"].newString("42");

        EXPECT_EQ(jsonStrZip, json.dump());
    }
    {
        mblet::Jsonator json;
        json["root"]["1"][0] = 0;
        json["root"]["1"][1] = 1;
        json["root"]["1"][2] = 2;
        json["root"]["2"] = true;
        json["root"]["3"].newNull();
        json["root"]["4"] = 42;
        json["root"]["5"]["0"] = 0;
        json["root"]["5"]["1"] = 1;
        json["root"]["6"] = "42";

        EXPECT_EQ(jsonStrZip, json.dump());
    }
    {
        mblet::Jsonator json;
        std::vector<int> vectorInt;
        vectorInt.push_back(0);
        vectorInt.push_back(1);
        vectorInt.push_back(2);
        std::map<std::string, int> mapInt;
        mapInt["0"] = 0;
        mapInt["1"] = 1;
        json["root"]["1"] = vectorInt;
        json["root"]["2"] = true;
        json["root"]["3"].newNull();
        json["root"]["4"] = 42;
        json["root"]["5"] = mapInt;
        json["root"]["6"] = "42";

        EXPECT_EQ(jsonStrZip, json.dump());
    }
    {
        mblet::Jsonator json;
        json["root"]["1"].push_back(2);
        json["root"]["1"].push_front(0);
        json["root"]["1"].insert(1, 1);
        json["root"]["2"] = true;
        json["root"]["3"].newNull();
        json["root"]["4"] = 42;
        json["root"]["5"]["0"] = 0;
        json["root"]["5"]["1"] = 1;
        json["root"]["6"] = "42";

        EXPECT_EQ(jsonStrZip, json.dump());
    }
}