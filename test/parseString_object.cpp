#include <gtest/gtest.h>

#include "mblet/jsonator.h"
#include "mock/jsonToString.h"

GTEST_TEST(parseString_object, empty) {
    mblet::Jsonator json = mblet::Jsonator::parseString("{}");
    EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
    EXPECT_EQ(json.size(), 0);
    json.parseString("{  }");
    EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
    EXPECT_EQ(json.size(), 0);
    json.parseString("{\t}");
    EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
    EXPECT_EQ(json.size(), 0);
    json.parseString("{\n}");
    EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
    EXPECT_EQ(json.size(), 0);
}

GTEST_TEST(parseString_object, null) {
    // clang-format off
    std::string jsonStr = JSON_TO_STRING((
        {
            "": null
        }
    ));
    // clang-format on

    mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
    EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
    EXPECT_EQ(json.size(), 1);
    EXPECT_EQ(json.hasKey(""), true);
    EXPECT_EQ(json.at("").getType(), mblet::Jsonator::NONE);
    EXPECT_EQ(json.at("").isNull(), true);
}

GTEST_TEST(parseString_object, boolean) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "": true
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey(""), true);
        EXPECT_EQ(json.at("").getType(), mblet::Jsonator::BOOLEAN);
        EXPECT_EQ(json.at("").isBoolean(), true);
        EXPECT_EQ(json.at("").getBoolean(), true);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "": false
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey(""), true);
        EXPECT_EQ(json.at("").getType(), mblet::Jsonator::BOOLEAN);
        EXPECT_EQ(json.at("").isBoolean(), true);
        EXPECT_EQ(json.at("").getBoolean(), false);
    }
}

GTEST_TEST(parseString_object, number) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "key": 42
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey("key"), true);
        EXPECT_EQ(json.at("key").getType(), mblet::Jsonator::NUMBER);
        EXPECT_EQ(json.at("key").isNumber(), true);
        EXPECT_EQ(json.at("key").getNumber(), 42);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "key": 42.42
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey("key"), true);
        EXPECT_EQ(json.at("key").getType(), mblet::Jsonator::NUMBER);
        EXPECT_EQ(json.at("key").isNumber(), true);
        EXPECT_EQ(json.at("key").getNumber(), 42.42);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "key": 1e6
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey("key"), true);
        EXPECT_EQ(json.at("key").getType(), mblet::Jsonator::NUMBER);
        EXPECT_EQ(json.at("key").isNumber(), true);
        EXPECT_EQ(json.at("key").getNumber(), 1e6);
    }
}

GTEST_TEST(parseString_object, string) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "key": "42"
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey("key"), true);
        EXPECT_EQ(json.at("key").getType(), mblet::Jsonator::STRING);
        EXPECT_EQ(json.at("key").isString(), true);
        EXPECT_EQ(json.at("key").getString(), "42");
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "key": "42\\\n\"42"
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey("key"), true);
        EXPECT_EQ(json.at("key").getType(), mblet::Jsonator::STRING);
        EXPECT_EQ(json.at("key").isString(), true);
        EXPECT_EQ(json.at("key").getString(), "42\\\n\"42");
    }
}

GTEST_TEST(parseString_object, array) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "key": []
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey("key"), true);
        EXPECT_EQ(json.at("key").getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.at("key").isArray(), true);
        EXPECT_EQ(json.at("key").size(), 0);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "key": [
                    42
                ]
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey("key"), true);
        EXPECT_EQ(json.at("key").getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.at("key").isArray(), true);
        EXPECT_EQ(json.at("key").size(), 1);
        EXPECT_EQ(json.at("key").at(0), 42);
    }
}

GTEST_TEST(parseString_object, object) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "key": {}
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey("key"), true);
        EXPECT_EQ(json.at("key").getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.at("key").isObject(), true);
        EXPECT_EQ(json.at("key").size(), 0);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            {
                "key": {
                    "child": 42
                }
            }
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.hasKey("key"), true);
        EXPECT_EQ(json.at("key").getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.at("key").isObject(), true);
        EXPECT_EQ(json.at("key").size(), 1);
        EXPECT_EQ(json.at("key").hasKey("child"), true);
        EXPECT_EQ(json.at("key").at("child"), 42);
    }
}