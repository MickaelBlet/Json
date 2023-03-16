#include <gtest/gtest.h>

#include "mblet/jsonator.h"
#include "mock/jsonToString.h"

GTEST_TEST(jsonator, find) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json.find("foo");
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NONE_TYPE).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                const mblet::Jsonator json;
                json.find("foo");
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NONE_TYPE).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json.find(0);
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NONE_TYPE).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                const mblet::Jsonator json;
                json.find(0);
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NONE_TYPE).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    {
        mblet::Jsonator json;
        json["foo"] = 42;
        EXPECT_EQ(json.find("foo")->first, "foo");
        EXPECT_EQ(json.find("foo")->second.getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json["foo"] = 42;
        const mblet::Jsonator& cjson = json;
        EXPECT_EQ(cjson.find("foo")->first, "foo");
        EXPECT_EQ(cjson.find("foo")->second.getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json[0] = 42;
        EXPECT_EQ(json.find(0)->getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json[0] = 42;
        const mblet::Jsonator& cjson = json;
        EXPECT_EQ(cjson.find(0)->getNumber(), 42);
    }
}

GTEST_TEST(jsonator, bracket) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                json["foo"];
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NUMBER_TYPE).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                const mblet::Jsonator& cjson = json;
                cjson["foo"];
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NUMBER_TYPE).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                json[0];
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NUMBER_TYPE).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                const mblet::Jsonator& cjson = json;
                cjson[0];
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NUMBER_TYPE).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json["foo"] = 42;
                const mblet::Jsonator& cjson = json;
                cjson["bar"];
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "has not a child 'bar'.");
                EXPECT_EQ(e.message(), "has not a child");
                EXPECT_EQ(e.child(), "bar");
                throw;
            }
        },
        mblet::Jsonator::ChildException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json[0] = 42;
                const mblet::Jsonator& cjson = json;
                cjson[1];
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "out of range '1'.");
                EXPECT_EQ(e.message(), "out of range");
                EXPECT_EQ(e.index(), 1);
                throw;
            }
        },
        mblet::Jsonator::ChildException);
    {
        mblet::Jsonator json;
        json["foo"] = 42;
        EXPECT_EQ(json["foo"].getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json["foo"] = 42;
        const mblet::Jsonator& cjson = json;
        EXPECT_EQ(cjson["foo"].getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json[0] = 42;
        EXPECT_EQ(json[0].getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json[0] = 42;
        const mblet::Jsonator& cjson = json;
        EXPECT_EQ(cjson[0].getNumber(), 42);
    }
}

GTEST_TEST(jsonator, at) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                json.at("foo");
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NUMBER_TYPE).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                const mblet::Jsonator& cjson = json;
                cjson.at("foo");
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NUMBER_TYPE).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                json.at(0);
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NUMBER_TYPE).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                const mblet::Jsonator& cjson = json;
                cjson.at(0);
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NUMBER_TYPE).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json["foo"] = 42;
                json.at("bar");
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "has not a child 'bar'.");
                EXPECT_EQ(e.message(), "has not a child");
                EXPECT_EQ(e.child(), "bar");
                throw;
            }
        },
        mblet::Jsonator::ChildException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json["foo"] = 42;
                const mblet::Jsonator& cjson = json;
                cjson.at("bar");
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "has not a child 'bar'.");
                EXPECT_EQ(e.message(), "has not a child");
                EXPECT_EQ(e.child(), "bar");
                throw;
            }
        },
        mblet::Jsonator::ChildException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json[0] = 42;
                json.at(1);
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "out of range '1'.");
                EXPECT_EQ(e.message(), "out of range");
                EXPECT_EQ(e.index(), 1);
                throw;
            }
        },
        mblet::Jsonator::ChildException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json[0] = 42;
                const mblet::Jsonator& cjson = json;
                cjson.at(1);
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "out of range '1'.");
                EXPECT_EQ(e.message(), "out of range");
                EXPECT_EQ(e.index(), 1);
                throw;
            }
        },
        mblet::Jsonator::ChildException);
}

GTEST_TEST(jsonator, front) {
    {
        mblet::Jsonator json;
        json.push_back(42);
        json.push_back(24);
        json.push_back(84);
        EXPECT_EQ(json.front(), 42);
    }
    {
        mblet::Jsonator json;
        json.push_back(42);
        json.push_back(24);
        json.push_back(84);
        const mblet::Jsonator& cjson = json;
        EXPECT_EQ(cjson.front(), 42);
    }
}

GTEST_TEST(jsonator, back) {
    {
        mblet::Jsonator json;
        json.push_back(42);
        json.push_back(24);
        json.push_back(84);
        EXPECT_EQ(json.back(), 84);
    }
    {
        mblet::Jsonator json;
        json.push_back(42);
        json.push_back(24);
        json.push_back(84);
        const mblet::Jsonator& cjson = json;
        EXPECT_EQ(cjson.back(), 84);
    }
}

GTEST_TEST(jsonator, isNull) {
    {
        mblet::Jsonator json;
        json.newNull();
        EXPECT_EQ(json.isNull(), true);
    }
    {
        mblet::Jsonator json;
        json.newNumber(42);
        EXPECT_EQ(json.isNull(), false);
    }
}

GTEST_TEST(jsonator, isObject) {
    {
        mblet::Jsonator json;
        json.newObject();
        EXPECT_EQ(json.isObject(), true);
    }
    {
        mblet::Jsonator json;
        json.newNumber(42);
        EXPECT_EQ(json.isObject(), false);
    }
}

GTEST_TEST(jsonator, isArray) {
    {
        mblet::Jsonator json;
        json.newArray();
        EXPECT_EQ(json.isArray(), true);
    }
    {
        mblet::Jsonator json;
        json.newNumber(42);
        EXPECT_EQ(json.isArray(), false);
    }
}

GTEST_TEST(jsonator, isString) {
    {
        mblet::Jsonator json;
        json.newString("foo");
        EXPECT_EQ(json.isString(), true);
    }
    {
        mblet::Jsonator json;
        json.newNumber(42);
        EXPECT_EQ(json.isString(), false);
    }
}

GTEST_TEST(jsonator, isNumber) {
    {
        mblet::Jsonator json;
        json.newNumber(42);
        EXPECT_EQ(json.isNumber(), true);
    }
    {
        mblet::Jsonator json;
        json.newNull();
        EXPECT_EQ(json.isNumber(), false);
    }
}

GTEST_TEST(jsonator, isBoolean) {
    {
        mblet::Jsonator json;
        json.newBoolean(true);
        EXPECT_EQ(json.isBoolean(), true);
    }
    {
        mblet::Jsonator json;
        json.newNumber(42);
        EXPECT_EQ(json.isBoolean(), false);
    }
}

GTEST_TEST(jsonator, contains) {
    {
        mblet::Jsonator json;
        json["foo"] = 1;
        json["bar"] = 2;
        EXPECT_EQ(json.contains("foo"), true);
        EXPECT_EQ(json.contains("bar"), true);
        EXPECT_EQ(json.contains("toto"), false);
    }
}

GTEST_TEST(jsonator, dump) {
    // clang-format off
    std::string jsonStr = JSON_TO_STRING((
        {
            "foo": [
                "bar"
            ],
            "bar": {
                "foo": 42
            }
        }
    ));
    // clang-format on
    mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
    for (unsigned int i = 0; i < 1000; ++i) {
        std::ostringstream oss("");
        oss << '{';
        if (i > 0) {
            oss << '\n';
            oss << std::string(i, '\t');
        }
        oss << "\"bar\":";
        if (i > 0) {
            oss << ' ';
        }
        oss << '{';
        if (i > 0) {
            oss << '\n';
            oss << std::string(i * 2, '\t');
        }
        oss << "\"foo\":";
        if (i > 0) {
            oss << ' ';
        }
        oss << 42;
        if (i > 0) {
            oss << '\n';
            oss << std::string(i, '\t');
        }
        oss << "},";
        if (i > 0) {
            oss << '\n';
            oss << std::string(i, '\t');
        }
        oss << "\"foo\":";
        if (i > 0) {
            oss << ' ';
        }
        oss << '[';
        if (i > 0) {
            oss << '\n';
            oss << std::string(i * 2, '\t');
        }
        oss << "\"bar\"";
        if (i > 0) {
            oss << '\n';
            oss << std::string(i, '\t');
        }
        oss << ']';
        if (i > 0) {
            oss << '\n';
        }
        oss << '}';
        EXPECT_EQ(json.dump(i, '\t'), oss.str());
    }
}

GTEST_TEST(jsonator, getString) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                json.getString();
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a string (is NUMBER_TYPE).");
                EXPECT_EQ(e.message(), "is not a string");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    {
        mblet::Jsonator json;
        json = "foo";
        std::string str = json.getString();
        EXPECT_EQ(str, "foo");
    }
}

GTEST_TEST(jsonator, getNumber) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = "42";
                json.getNumber();
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a number (is STRING_TYPE).");
                EXPECT_EQ(e.message(), "is not a number");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    {
        mblet::Jsonator json;
        json = 42.42;
        double d = json.getNumber();
        EXPECT_EQ(d, 42.42);
    }
}

GTEST_TEST(jsonator, getBoolean) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = "42";
                json.getBoolean();
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a boolean (is STRING_TYPE).");
                EXPECT_EQ(e.message(), "is not a boolean");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    {
        mblet::Jsonator json;
        json = true;
        bool b = json.getBoolean();
        EXPECT_EQ(b, true);
    }
}

GTEST_TEST(jsonator, getType) {
    {
        mblet::Jsonator json;
        json["object"].newObject();
        json["array"].newArray();
        json["null"].newNull();
        json["boolean"].newBoolean(true);
        json["number"].newNumber(42);
        json["string"].newString("foo");
        EXPECT_EQ(json["object"].getType(), mblet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json["array"].getType(), mblet::Jsonator::ARRAY_TYPE);
        EXPECT_EQ(json["null"].getType(), mblet::Jsonator::NONE_TYPE);
        EXPECT_EQ(json["boolean"].getType(), mblet::Jsonator::BOOLEAN_TYPE);
        EXPECT_EQ(json["number"].getType(), mblet::Jsonator::NUMBER_TYPE);
        EXPECT_EQ(json["string"].getType(), mblet::Jsonator::STRING_TYPE);
    }
}

GTEST_TEST(jsonator, castOperator) {
    // clang-format off
    std::string jsonStr = JSON_TO_STRING((
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
    ));
    // clang-format on

    mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);

    std::string string = json.at("string");
    const char* starChar = json.at("starChar");
    bool boolean = json.at("boolean");
    std::deque<int> deque = json.at("deque");
    std::list<int> list = json.at("list");
    std::map<std::string, int> map = json.at("map");
    std::queue<int> queue = json.at("queue");
    std::set<int> set = json.at("set");
    std::stack<int> stack = json.at("stack");
    std::vector<int> vector = json.at("vector");
    char c = json.at("c");
    unsigned char uc = json.at("uc");
    short s = json.at("s");
    unsigned short us = json.at("us");
    int i = json.at("i");
    unsigned int ui = json.at("ui");
    long l = json.at("l");
    unsigned long ul = json.at("ul");
    float f = json.at("f");
    double d = json.at("d");

    EXPECT_EQ(string, "string");
    EXPECT_STREQ(starChar, "string");
    EXPECT_EQ(boolean, true);
    {
        std::deque<int>::iterator it = deque.begin();
        EXPECT_EQ(*it, 1337);
        ++it;
        EXPECT_EQ(*it, 42);
    }
    {
        std::list<int>::iterator it = list.begin();
        EXPECT_EQ(*it, 1337);
        ++it;
        EXPECT_EQ(*it, 42);
    }
    {
        std::map<std::string, int>::iterator it = map.begin();
        EXPECT_EQ(it->first, "bar");
        EXPECT_EQ(it->second, 42);
        ++it;
        EXPECT_EQ(it->first, "foo");
        EXPECT_EQ(it->second, 1337);
    }
    {
        EXPECT_EQ(queue.front(), 1337);
        queue.pop();
        EXPECT_EQ(queue.front(), 42);
    }
    {
        std::set<int>::iterator it = set.begin();
        EXPECT_EQ(*it, 42);
        ++it;
        EXPECT_EQ(*it, 1337);
    }
    {
        EXPECT_EQ(stack.top(), 1337);
        stack.pop();
        EXPECT_EQ(stack.top(), 42);
    }
    {
        std::vector<int>::iterator it = vector.begin();
        EXPECT_EQ(*it, 1337);
        ++it;
        EXPECT_EQ(*it, 42);
    }
    EXPECT_EQ(c, 42);
    EXPECT_EQ(uc, 42);
    EXPECT_EQ(s, 42);
    EXPECT_EQ(us, 42);
    EXPECT_EQ(i, 42);
    EXPECT_EQ(ui, 42);
    EXPECT_EQ(l, 42);
    EXPECT_EQ(ul, 42);
    EXPECT_EQ(f, 42);
    EXPECT_EQ(d, 42);
}