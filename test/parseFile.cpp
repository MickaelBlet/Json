#include <gtest/gtest.h>

#include "mblet/jsonator.h"
#include "mock/fileGuard.h"
#include "mock/jsonToString.h"

GTEST_TEST(parseFile, except) {
    const char* testFile = "/tmp/mblet_test_parseFile_except.json";
    {
        // create example file
        mblet::FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    } // remove file

    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json = mblet::Jsonator::parseFile(testFile);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse /tmp/mblet_test_parseFile_except.json: (Open file failed).");
                EXPECT_EQ(e.message(), "Open file failed");
                EXPECT_EQ(e.filename(), "/tmp/mblet_test_parseFile_except.json");
                EXPECT_EQ(e.line(), 0);
                EXPECT_EQ(e.column(), 0);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseFile, success) {
    // clang-format off
    std::string jsonStr = JSON_TO_STRING((
        {
            "key" : "value",
            "array": [
                0,1,2,3,4,5,6,7,8,9,10
            ]
        }
    ));
    // clang-format on

    // create example file
    const char* testFile = "/tmp/mblet_test_parsefile_success.json";
    mblet::FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    fileGuard << jsonStr << std::flush;
    fileGuard.close();

    mblet::Jsonator json = mblet::Jsonator::parseFile(testFile);
    EXPECT_EQ(json.contains("key"), true);
    EXPECT_EQ(json.at("key").getString(), "value");
    EXPECT_EQ(json.contains("array"), true);
    EXPECT_EQ(json.at("array").at(0).getNumber(), 0);
    EXPECT_EQ(json.at("array").at(1).getNumber(), 1);
    EXPECT_EQ(json.at("array").at(2).getNumber(), 2);
    EXPECT_EQ(json.at("array").at(3).getNumber(), 3);
    EXPECT_EQ(json.at("array").at(4).getNumber(), 4);
    EXPECT_EQ(json.at("array").at(5).getNumber(), 5);
    EXPECT_EQ(json.at("array").at(6).getNumber(), 6);
    EXPECT_EQ(json.at("array").at(7).getNumber(), 7);
    EXPECT_EQ(json.at("array").at(8).getNumber(), 8);
    EXPECT_EQ(json.at("array").at(9).getNumber(), 9);
    EXPECT_EQ(json.at("array").at(10).getNumber(), 10);
}