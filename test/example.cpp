#include <gtest/gtest.h>

#include "mblet/jsonator.h"
#include "mock/jsonToString.h"

GTEST_TEST(jsonator, test1) {
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

    mblet::Jsonator jsonator = mblet::Jsonator::parseString(jsonStr);
    char test[] = "example";
    const char* const test2 = "example";
    char test3[] = "array";
    std::cout << __LINE__ << std::endl;
    EXPECT_EQ(jsonator[test].getType(), mblet::Jsonator::OBJECT);
    std::cout << __LINE__ << std::endl;
    EXPECT_EQ(jsonator[test][test3].getType(), mblet::Jsonator::ARRAY);
    std::cout << __LINE__ << std::endl;
    EXPECT_EQ(jsonator[test][test3][0][0].getNumber(), 0);
    std::cout << __LINE__ << std::endl;
    EXPECT_EQ(jsonator[test2]["array"][0][1].getNumber(), 1);
    std::cout << __LINE__ << std::endl;
    EXPECT_EQ(jsonator["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonator["example"]["array"][1].getNumber(), 1);
    EXPECT_EQ(jsonator["example"]["array"][2].getNumber(), 2);
    EXPECT_EQ(jsonator["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonator["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonator["example"]["bool1"].getBoolean(), false);
    EXPECT_EQ(jsonator["example"]["bool2"].getBoolean(), true);
    EXPECT_EQ(jsonator["example"]["n\"one"].getType(), mblet::Jsonator::NONE);
    mblet::Jsonator jsonatorCpy = mblet::Jsonator::parseString(jsonator.dump(0));
    EXPECT_EQ(jsonatorCpy["example"].getKey(), "example");
    EXPECT_EQ(jsonatorCpy["example"]["array"].getKey(), "array");
    EXPECT_EQ(jsonatorCpy["example"]["array"][0].getKey(), "0");
    EXPECT_EQ(jsonatorCpy["example"]["array"][0][0].getKey(), "0");
    EXPECT_EQ(jsonatorCpy["example"]["array"][0][0].getNumber(), 0);
    EXPECT_EQ(jsonatorCpy["example"]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonatorCpy["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonatorCpy["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonatorCpy["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonatorCpy["example"]["bool1"].getBoolean(), false);
    EXPECT_EQ(jsonatorCpy["example"]["bool2"].getBoolean(), true);
    EXPECT_EQ(jsonatorCpy["example"]["n\"one"].getType(), mblet::Jsonator::NONE);
    std::cout << jsonatorCpy.dump(2) << std::endl;
    std::cout << jsonatorCpy.dump() << std::endl;
}

GTEST_TEST(jsonator, test2) {
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

    mblet::Jsonator jsonator = mblet::Jsonator::parseString(jsonStr);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][0].getNumber(), 0);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonator[0]["example"]["array"][1].getNumber(), 1);
    EXPECT_EQ(jsonator[0]["example"]["array"][2].getNumber(), 2);
    EXPECT_EQ(jsonator[0]["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonator[0]["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonator[0]["example"]["bool"].getBoolean(), false);
    EXPECT_EQ(jsonator[0]["example"]["n\"one"].getType(), mblet::Jsonator::NONE);
    mblet::Jsonator jsonatorCpy = mblet::Jsonator::parseString(jsonator.dump(0));
    EXPECT_EQ(jsonatorCpy[0]["example"].getKey(), "example");
    EXPECT_EQ(jsonatorCpy[0]["example"]["array"].getKey(), "array");
    EXPECT_EQ(jsonatorCpy[0]["example"]["array"][0].getKey(), "0");
    EXPECT_EQ(jsonatorCpy[0]["example"]["array"][0][0].getKey(), "0");
    EXPECT_EQ(jsonatorCpy[0]["example"]["array"][0][0].getNumber(), 0);
    EXPECT_EQ(jsonatorCpy[0]["example"]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonatorCpy[0]["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonatorCpy[0]["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonatorCpy[0]["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonatorCpy[0]["example"]["bool"].getBoolean(), false);
    EXPECT_EQ(jsonatorCpy[0]["example"]["n\"one"].getType(), mblet::Jsonator::NONE);
    std::cout << jsonatorCpy.dump(2) << std::endl;
    std::cout << jsonatorCpy.dump() << std::endl;
}