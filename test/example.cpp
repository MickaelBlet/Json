#include <gtest/gtest.h>

#include "blet/json.h"
#include "mock/jsonToString.h"

GTEST_TEST(example, test1) {
    // clang-format off
    // create example string
    std::string jsonStr = JSON_TO_STRING((
        {
            "example": {
                "array": [
                    [
                        0,
                        1,
                        2
                    ],
                    1,
                    2
                ],
                "string": "foo\nbar",
                "number": 42.42,
                "bool1": false,
                "bool2": true,
                "n\"one": null
            }
        }
    ));
    // clang-format on

    blet::Dict jsonator = blet::json::loadString(jsonStr);
    char test[] = "example";
    const char* const test2 = "example";
    char test3[] = "array";
    EXPECT_EQ(jsonator[test].getType(), blet::Dict::OBJECT_TYPE);
    EXPECT_EQ(jsonator[test][test3].getType(), blet::Dict::ARRAY_TYPE);
    EXPECT_EQ(jsonator[test][test3][0][0].getNumber(), 0);
    EXPECT_EQ(jsonator[test2]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonator["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonator["example"]["array"][1].getNumber(), 1);
    EXPECT_EQ(jsonator["example"]["array"][2].getNumber(), 2);
    EXPECT_EQ(jsonator["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonator["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonator["example"]["bool1"].getBoolean(), false);
    EXPECT_EQ(jsonator["example"]["bool2"].getBoolean(), true);
    EXPECT_EQ(jsonator["example"]["n\"one"].getType(), blet::Dict::NULL_TYPE);
    blet::Dict jsonatorCpy = blet::json::loadString(blet::json::dump(jsonator, 0));
    EXPECT_EQ(jsonatorCpy["example"]["array"][0][0].getNumber(), 0);
    EXPECT_EQ(jsonatorCpy["example"]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonatorCpy["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonatorCpy["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonatorCpy["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonatorCpy["example"]["bool1"].getBoolean(), false);
    EXPECT_EQ(jsonatorCpy["example"]["bool2"].getBoolean(), true);
    EXPECT_EQ(jsonatorCpy["example"]["n\"one"].getType(), blet::Dict::NULL_TYPE);
    std::cout << blet::json::dump(jsonatorCpy, 2) << std::endl;
    std::cout << blet::json::dump(jsonatorCpy) << std::endl;
}

GTEST_TEST(example, test2) {
    // clang-format off
    // create example string
    std::string jsonStr = JSON_TO_STRING((
        [
            {
                "example": {
                    "array": [
                        [
                            0,
                            1,
                            2
                        ],
                        1,
                        2
                    ],
                    "string": "foo\nbar",
                    "number": 42.42,
                    "bool": false,
                    "n\"one": null
                }
            }
        ]
    ));
    // clang-format on

    blet::Dict jsonator = blet::json::loadString(jsonStr);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][0].getNumber(), 0);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonator[0]["example"]["array"][1].getNumber(), 1);
    EXPECT_EQ(jsonator[0]["example"]["array"][2].getNumber(), 2);
    EXPECT_EQ(jsonator[0]["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonator[0]["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonator[0]["example"]["bool"].getBoolean(), false);
    EXPECT_EQ(jsonator[0]["example"]["n\"one"].getType(), blet::Dict::NULL_TYPE);
    blet::Dict jsonatorCpy = blet::json::loadString(blet::json::dump(jsonator, 0));
    EXPECT_EQ(jsonatorCpy[0]["example"]["array"][0][0].getNumber(), 0);
    EXPECT_EQ(jsonatorCpy[0]["example"]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonatorCpy[0]["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonatorCpy[0]["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonatorCpy[0]["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonatorCpy[0]["example"]["bool"].getBoolean(), false);
    EXPECT_EQ(jsonatorCpy[0]["example"]["n\"one"].getType(), blet::Dict::NULL_TYPE);
    std::cout << blet::json::dump(jsonatorCpy, 2) << std::endl;
    std::cout << blet::json::dump(jsonatorCpy) << std::endl;
}

GTEST_TEST(example, test3) {
    // clang-format off
    // create example string
    std::string jsonStr(
        "/* comment */"
        "{\"key\": \"valu\\\"\ne\"}"
    );
    // clang-format on

    EXPECT_THROW(
        {
            try {
                blet::Dict dict = blet::json::loadString(jsonStr, true);
            }
            catch (const blet::json::LoadException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:23 (New line in string).");
                EXPECT_EQ(e.message(), "New line in string");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 23);
                throw;
            }
        },
        blet::json::LoadException);
}
