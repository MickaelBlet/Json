#include <gtest/gtest.h>

#include "mblet/jsonator.h"
#include "mock/jsonToString.h"

GTEST_TEST(parseString_object, empty) {
    blet::Jsonator json = blet::Jsonator::parseString("{}");
    EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
    EXPECT_EQ(json.size(), 0);
    json.parseString("{  }");
    EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
    EXPECT_EQ(json.size(), 0);
    json.parseString("{\t}");
    EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
    EXPECT_EQ(json.size(), 0);
    json.parseString("{\n}");
    EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
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

    blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
    EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
    EXPECT_EQ(json.size(), 1);
    EXPECT_EQ(json.contains(""), true);
    EXPECT_EQ(json.at("").getType(), blet::Jsonator::NULL_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains(""), true);
        EXPECT_EQ(json.at("").getType(), blet::Jsonator::BOOLEAN_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains(""), true);
        EXPECT_EQ(json.at("").getType(), blet::Jsonator::BOOLEAN_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains("key"), true);
        EXPECT_EQ(json.at("key").getType(), blet::Jsonator::NUMBER_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains("key"), true);
        EXPECT_EQ(json.at("key").getType(), blet::Jsonator::NUMBER_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains("key"), true);
        EXPECT_EQ(json.at("key").getType(), blet::Jsonator::NUMBER_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains("key"), true);
        EXPECT_EQ(json.at("key").getType(), blet::Jsonator::STRING_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains("key"), true);
        EXPECT_EQ(json.at("key").getType(), blet::Jsonator::STRING_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains("key"), true);
        EXPECT_EQ(json.at("key").getType(), blet::Jsonator::ARRAY_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains("key"), true);
        EXPECT_EQ(json.at("key").getType(), blet::Jsonator::ARRAY_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains("key"), true);
        EXPECT_EQ(json.at("key").getType(), blet::Jsonator::OBJECT_TYPE);
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

        blet::Jsonator json = blet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.contains("key"), true);
        EXPECT_EQ(json.at("key").getType(), blet::Jsonator::OBJECT_TYPE);
        EXPECT_EQ(json.at("key").isObject(), true);
        EXPECT_EQ(json.at("key").size(), 1);
        EXPECT_EQ(json.at("key").contains("child"), true);
        EXPECT_EQ(json.at("key").at("child"), 42);
    }
}