#include <gtest/gtest.h>

#include "mblet/jsonator.h"
#include "mock/jsonToString.h"

GTEST_TEST(parseString_array, empty) {
    mblet::Jsonator json = mblet::Jsonator::parseString("[]");
    EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
    EXPECT_EQ(json.size(), 0);
    json.parseString("[  ]");
    EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
    EXPECT_EQ(json.size(), 0);
    json.parseString("[\t]");
    EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
    EXPECT_EQ(json.size(), 0);
    json.parseString("[\n]");
    EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
    EXPECT_EQ(json.size(), 0);
}

GTEST_TEST(parseString_array, null) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                null
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::NONE);
        EXPECT_EQ(json.at(0).isNull(), true);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                null,
                null
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 2);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::NONE);
        EXPECT_EQ(json.at(0).isNull(), true);
        EXPECT_EQ(json.at(1).getType(), mblet::Jsonator::NONE);
        EXPECT_EQ(json.at(1).isNull(), true);
    }
}

GTEST_TEST(parseString_array, boolean) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                true
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::BOOLEAN);
        EXPECT_EQ(json.at(0).isBoolean(), true);
        EXPECT_EQ(json.at(0).getBoolean(), true);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                false
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::BOOLEAN);
        EXPECT_EQ(json.at(0).isBoolean(), true);
        EXPECT_EQ(json.at(0).getBoolean(), false);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                true,
                false
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 2);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::BOOLEAN);
        EXPECT_EQ(json.at(0).isBoolean(), true);
        EXPECT_EQ(json.at(0).getBoolean(), true);
        EXPECT_EQ(json.at(1).getType(), mblet::Jsonator::BOOLEAN);
        EXPECT_EQ(json.at(1).isBoolean(), true);
        EXPECT_EQ(json.at(1).getBoolean(), false);
    }
}

GTEST_TEST(parseString_array, number) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                42
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::NUMBER);
        EXPECT_EQ(json.at(0).isNumber(), true);
        EXPECT_EQ(json.at(0).getNumber(), 42);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                42.42
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::NUMBER);
        EXPECT_EQ(json.at(0).isNumber(), true);
        EXPECT_EQ(json.at(0).getNumber(), 42.42);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                1e6
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::NUMBER);
        EXPECT_EQ(json.at(0).isNumber(), true);
        EXPECT_EQ(json.at(0).getNumber(), 1e6);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                42,
                42.42,
                1e6
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 3);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::NUMBER);
        EXPECT_EQ(json.at(0).isNumber(), true);
        EXPECT_EQ(json.at(0).getNumber(), 42);
        EXPECT_EQ(json.at(1).getType(), mblet::Jsonator::NUMBER);
        EXPECT_EQ(json.at(1).isNumber(), true);
        EXPECT_EQ(json.at(1).getNumber(), 42.42);
        EXPECT_EQ(json.at(2).getType(), mblet::Jsonator::NUMBER);
        EXPECT_EQ(json.at(2).isNumber(), true);
        EXPECT_EQ(json.at(2).getNumber(), 1e6);
    }
}

GTEST_TEST(parseString_array, string) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                "42"
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::STRING);
        EXPECT_EQ(json.at(0).isString(), true);
        EXPECT_EQ(json.at(0).getString(), "42");
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                "42\n42"
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::STRING);
        EXPECT_EQ(json.at(0).isString(), true);
        EXPECT_EQ(json.at(0).getString(), "42\n42");
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                "42",
                "42\n42"
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 2);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::STRING);
        EXPECT_EQ(json.at(0).isString(), true);
        EXPECT_EQ(json.at(0).getString(), "42");
        EXPECT_EQ(json.at(1).getType(), mblet::Jsonator::STRING);
        EXPECT_EQ(json.at(1).isString(), true);
        EXPECT_EQ(json.at(1).getString(), "42\n42");
    }
}

GTEST_TEST(parseString_array, array) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                []
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.at(0).isArray(), true);
        EXPECT_EQ(json.at(0).size(), 0);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                [
                    42
                ]
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.at(0).isArray(), true);
        EXPECT_EQ(json.at(0).size(), 1);
        EXPECT_EQ(json.at(0).at(0), 42);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                [],
                [
                    42
                ]
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 2);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.at(0).isArray(), true);
        EXPECT_EQ(json.at(0).size(), 0);
        EXPECT_EQ(json.at(1).getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.at(1).isArray(), true);
        EXPECT_EQ(json.at(1).size(), 1);
        EXPECT_EQ(json.at(1).at(0), 42);
    }
}

GTEST_TEST(parseString_array, object) {
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                {}
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.at(0).isObject(), true);
        EXPECT_EQ(json.at(0).size(), 0);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                {
                    "child": 42
                }
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 1);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.at(0).isObject(), true);
        EXPECT_EQ(json.at(0).size(), 1);
        EXPECT_EQ(json.at(0).hasKey("child"), true);
        EXPECT_EQ(json.at(0).at("child"), 42);
    }
    {
        // clang-format off
        std::string jsonStr = JSON_TO_STRING((
            [
                {},
                {
                    "child": 42
                }
            ]
        ));
        // clang-format on

        mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
        EXPECT_EQ(json.getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(json.size(), 2);
        EXPECT_EQ(json.at(0).getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.at(0).isObject(), true);
        EXPECT_EQ(json.at(0).size(), 0);
        EXPECT_EQ(json.at(1).getType(), mblet::Jsonator::OBJECT);
        EXPECT_EQ(json.at(1).isObject(), true);
        EXPECT_EQ(json.at(1).size(), 1);
        EXPECT_EQ(json.at(1).hasKey("child"), true);
        EXPECT_EQ(json.at(1).at("child"), 42);
    }
}

GTEST_TEST(parseString_array, recurse) {
    const unsigned int numberOfRecurse = 1000;

    std::string jsonStr = std::string(numberOfRecurse, '[') + "42" + std::string(numberOfRecurse, ']');

    mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
    const mblet::Jsonator* jsonRecurse = &json;
    for (unsigned int i = 0; i < numberOfRecurse - 1; ++i) {
        EXPECT_EQ(jsonRecurse->getType(), mblet::Jsonator::ARRAY);
        EXPECT_EQ(jsonRecurse->size(), 1);
        jsonRecurse = &(jsonRecurse->at(0));
    }
    EXPECT_EQ(jsonRecurse->at(0).getType(), mblet::Jsonator::NUMBER);
    EXPECT_EQ(jsonRecurse->at(0).isNumber(), true);
    EXPECT_EQ(jsonRecurse->at(0).getNumber(), 42);
}

GTEST_TEST(parseString_array, multivalues) {
    // clang-format off
    std::string jsonStr = JSON_TO_STRING((
        [
            0,1,2,3,4,5,6,7,8,9,
            0,1,2,3,4,5,6,7,8,9,
            0,1,2,3,4,5,6,7,8,9,
            0,1,2,3,4,5,6,7,8,9,
            0,1,2,3,4,5,6,7,8,9,
            0,1,2,3,4,5,6,7,8,9,
            0,1,2,3,4,5,6,7,8,9,
            0,1,2,3,4,5,6,7,8,9,
            0,1,2,3,4,5,6,7,8,9,
            0,1,2,3,4,5,6,7,8,9,
            100
        ]
    ));
    // clang-format on

    mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
    for (unsigned int i = 0; i < 100; ++i) {
        EXPECT_EQ(json[i].getNumber(), i % 10);
    }
    EXPECT_EQ(json[100].getNumber(), 100);
}