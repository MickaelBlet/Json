#include <gtest/gtest.h>

#include "mblet/jsonator.h"

GTEST_TEST(jsonator, test1) {
    // create example string
    std::string
    strConf("{"
            "  \"example\": {"
            "    \"array\": ["
            "      ["
            "        0,"
            "        1,"
            "        2"
            "      ],"
            "      1,"
            "      2"
            "    ],"
            "    \"string\": \"foo\\nbar\","
            "    \"number\": 42.42,"
            "    \"bool\": false,"
            "    \"n\\\"one\": null"
            "  }"
            "}");

    mblet::Jsonator jsonator;
    jsonator.parseString(strConf);
    char test[] = "example";
    const char *const test2 = "example";
    char test3[] = "array";
    EXPECT_EQ(jsonator[test][test3][0][0].getNumber(), 0);
    EXPECT_EQ(jsonator[test2]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonator["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonator["example"]["array"][1].getNumber(), 1);
    EXPECT_EQ(jsonator["example"]["array"][2].getNumber(), 2);
    EXPECT_EQ(jsonator["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonator["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonator["example"]["bool"].getBoolean(), false);
    EXPECT_EQ(jsonator["example"]["n\"one"].getType(), mblet::Jsonator::Map::NONE);
    std::string str(jsonator.dump(0));
    std::cout << str << std::endl;
    jsonator.parseString(str);
    EXPECT_EQ(jsonator["example"].getKey(), "example");
    EXPECT_EQ(jsonator["example"]["array"].getKey(), "array");
    EXPECT_EQ(jsonator["example"]["array"][0].getKey(), "0");
    EXPECT_EQ(jsonator["example"]["array"][0][0].getKey(), "0");
    EXPECT_EQ(jsonator["example"]["array"][0][0].getNumber(), 0);
    EXPECT_EQ(jsonator["example"]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonator["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonator["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonator["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonator["example"]["bool"].getBoolean(), false);
    EXPECT_EQ(jsonator["example"]["n\"one"].getType(), mblet::Jsonator::Map::NONE);
    std::cout << jsonator.dump(2) << std::endl;
    std::cout << jsonator.dump() << std::endl;
}

GTEST_TEST(jsonator, test2) {
    // create example string
    std::string
    strConf("[{\"example\": {\"array\": [[0, 1, 2], 1, 2],\"string\": \"foo\\nbar\",\"number\": 42.42,\"bool\": false,\"n\\\"one\": null}}]");

    mblet::Jsonator jsonator;
    jsonator.parseString(strConf);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][0].getNumber(), 0);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonator[0]["example"]["array"][1].getNumber(), 1);
    EXPECT_EQ(jsonator[0]["example"]["array"][2].getNumber(), 2);
    EXPECT_EQ(jsonator[0]["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonator[0]["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonator[0]["example"]["bool"].getBoolean(), false);
    EXPECT_EQ(jsonator[0]["example"]["n\"one"].getType(), mblet::Jsonator::Map::NONE);
    std::string str(jsonator.dump(0));
    std::cout << str << std::endl;
    jsonator.parseString(str);
    EXPECT_EQ(jsonator[0]["example"].getKey(), "example");
    EXPECT_EQ(jsonator[0]["example"]["array"].getKey(), "array");
    EXPECT_EQ(jsonator[0]["example"]["array"][0].getKey(), "0");
    EXPECT_EQ(jsonator[0]["example"]["array"][0][0].getKey(), "0");
    EXPECT_EQ(jsonator[0]["example"]["array"][0][0].getNumber(), 0);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][1].getNumber(), 1);
    EXPECT_EQ(jsonator[0]["example"]["array"][0][2].getNumber(), 2);
    EXPECT_EQ(jsonator[0]["example"]["string"].getString(), "foo\nbar");
    EXPECT_EQ(jsonator[0]["example"]["number"].getNumber(), 42.42);
    EXPECT_EQ(jsonator[0]["example"]["bool"].getBoolean(), false);
    EXPECT_EQ(jsonator[0]["example"]["n\"one"].getType(), mblet::Jsonator::Map::NONE);
    std::cout << jsonator.dump(2) << std::endl;
    std::cout << jsonator.dump() << std::endl;
}