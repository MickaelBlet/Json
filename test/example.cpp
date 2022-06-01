#include <gtest/gtest.h>

#include "jsonator.h"

GTEST_TEST(jsonator, test1) {
    // create example string
    std::string strConf("{\"example\": {\"table\": [[0, 1, 2], 1, 2],\"string\": \"foo\\nbar\",\"number\": 42.42,\"bool\": false,\"n\\\"one\": null}}");

    mblet::Jsonator jsonator;
    jsonator.readString(strConf);
    EXPECT_EQ(jsonator["example"]["table"][0][0].number, 0);
    EXPECT_EQ(jsonator["example"]["table"][0][1].number, 1);
    EXPECT_EQ(jsonator["example"]["table"][0][2].number, 2);
    EXPECT_EQ(jsonator["example"]["table"][1].number, 1);
    EXPECT_EQ(jsonator["example"]["table"][2].number, 2);
    EXPECT_EQ(jsonator["example"]["string"].str, "foo\nbar");
    EXPECT_EQ(jsonator["example"]["number"].number, 42.42);
    EXPECT_EQ(jsonator["example"]["bool"].boolean, false);
    EXPECT_EQ(jsonator["example"]["n\"one"].type, mblet::Jsonator::Map::NONE);
    std::ostringstream oss("");
    jsonator.dump(0, oss);
    std::cout << oss.str() << std::endl;
    jsonator.readString(oss.str());
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
    jsonator.dump(2);
    jsonator.dump(0);
}