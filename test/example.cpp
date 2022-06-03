#include <gtest/gtest.h>

#include "mblet/jsonator.h"

GTEST_TEST(jsonator, test1) {
    // create example string
    std::string
    strConf("{\"example\": {\"table\": [[0, 1, 2], 1, 2],\"string\": \"foo\\nbar\",\"number\": 42.42,\"bool\": false,\"n\\\"one\": null}}");

    mblet::Jsonator jsonator;
    jsonator.parseString(strConf);
    EXPECT_EQ(jsonator["example"]["table"][0][0].number, 0);
    EXPECT_EQ(jsonator["example"]["table"][0][1].number, 1);
    EXPECT_EQ(jsonator["example"]["table"][0][2].number, 2);
    EXPECT_EQ(jsonator["example"]["table"][1].number, 1);
    EXPECT_EQ(jsonator["example"]["table"][2].number, 2);
    EXPECT_EQ(jsonator["example"]["string"].str, "foo\nbar");
    EXPECT_EQ(jsonator["example"]["number"].number, 42.42);
    EXPECT_EQ(jsonator["example"]["bool"].boolean, false);
    EXPECT_EQ(jsonator["example"]["n\"one"].type, mblet::Jsonator::Map::NONE);
    std::string str(jsonator.dump(0));
    std::cout << str << std::endl;
    jsonator.parseString(str);
    EXPECT_EQ(jsonator["example"].key, "example");
    EXPECT_EQ(jsonator["example"].key, "example");
    EXPECT_EQ(jsonator["example"]["table"].key, "table");
    EXPECT_EQ(jsonator["example"]["table"][0].key, "table");
    EXPECT_EQ(jsonator["example"]["table"][0][0].key, "table");
    EXPECT_EQ(jsonator["example"]["table"][0][0].number, 0);
    EXPECT_EQ(jsonator["example"]["table"][0][1].number, 1);
    EXPECT_EQ(jsonator["example"]["table"][0][2].number, 2);
    EXPECT_EQ(jsonator["example"]["string"].str, "foo\nbar");
    EXPECT_EQ(jsonator["example"]["number"].number, 42.42);
    EXPECT_EQ(jsonator["example"]["bool"].boolean, false);
    EXPECT_EQ(jsonator["example"]["n\"one"].type, mblet::Jsonator::Map::NONE);
    std::cout << jsonator.dump(2) << std::endl;
    std::cout << jsonator.dump(0) << std::endl;
}