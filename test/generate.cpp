#include <gtest/gtest.h>

#include "mblet/jsonator.h"
#include "mock/jsonToString.h"

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
    std::string jsonStr = JSON_TO_STRING(({
        "arrayChar" : "string",
        "boolean" : true,
        "c" : 42,
        "d" : 42,
        "deque" : [ 1337, 42 ],
        "f" : 42,
        "i" : 42,
        "jsonator" : null,
        "l" : 42,
        "list" : [ 1337, 42 ],
        "map" : {"bar" : 42, "foo" : 1337},
        "queue" : [ 1337, 42 ],
        "s" : 42,
        "set" : [ 42, 1337 ],
        "stack" : [ 42, 1337 ],
        "starChar" : "string",
        "string" : "string",
        "uc" : 42,
        "ui" : 42,
        "ul" : 42,
        "us" : 42,
        "vector" : [ 1337, 42 ]
    }));
    // clang-format on

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

    EXPECT_EQ(jsonStr, json.dump());
}

GTEST_TEST(jsonator, insert) {
    mblet::Jsonator json;
    json.insert("foo", "bar");
    json["bar"].insert(0, "bar");
    json["bar"].insert(0, "foo");
    EXPECT_EQ(json["foo"].getString(), "bar");
    EXPECT_EQ(json["bar"][0].getString(), "foo");
    EXPECT_EQ(json["bar"][1].getString(), "bar");
}

GTEST_TEST(jsonator, push_front) {
    mblet::Jsonator json;
    json.push_front("foo");
    json.push_front("bar");
    EXPECT_EQ(json.size(), 2);
    EXPECT_EQ(json[0].getString(), "bar");
    EXPECT_EQ(json[1].getString(), "foo");
}

GTEST_TEST(jsonator, push_back) {
    mblet::Jsonator json;
    json.push_back("foo");
    json.push_back("bar");
    EXPECT_EQ(json.size(), 2);
    EXPECT_EQ(json[0].getString(), "foo");
    EXPECT_EQ(json[1].getString(), "bar");
}

GTEST_TEST(jsonator, pop_front) {
    mblet::Jsonator json;
    json.push_back("foo");
    json.push_back("bar");
    json.pop_front();
    EXPECT_EQ(json.size(), 1);
    EXPECT_EQ(json.front().getString(), "bar");
}

GTEST_TEST(jsonator, pop_back) {
    mblet::Jsonator json;
    json.push_back("foo");
    json.push_back("bar");
    json.pop_back();
    EXPECT_EQ(json.size(), 1);
    EXPECT_EQ(json.front().getString(), "foo");
}

GTEST_TEST(jsonator, newObject) {}

GTEST_TEST(jsonator, newArray) {}

GTEST_TEST(jsonator, newString) {}

GTEST_TEST(jsonator, newNumber) {}

GTEST_TEST(jsonator, newBoolean) {}

GTEST_TEST(jsonator, newNull) {}

GTEST_TEST(jsonator, erase) {}

GTEST_TEST(jsonator, clear) {}

GTEST_TEST(jsonator, generate) {
    // clang-format off
    std::string jsonStr = JSON_TO_STRING((
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
    ));
    // clang-format on

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

        EXPECT_EQ(jsonStr, json.dump());
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

        EXPECT_EQ(jsonStr, json.dump());
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

        EXPECT_EQ(jsonStr, json.dump());
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

        EXPECT_EQ(jsonStr, json.dump());
    }
}