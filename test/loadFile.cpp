#include <gtest/gtest.h>

#include "blet/json.h"
#include "mock/fileGuard.h"
#include "mock/jsonToString.h"

GTEST_TEST(loadFile, except_open_file) {
    const char* testFile = "/tmp/blet_test_loadFile_except_open_file.json";
    {
        // create example file
        test::blet::FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    } // remove file

    EXPECT_THROW(
        {
            try {
                blet::Dict json = blet::json::loadFile(testFile);
            }
            catch (const blet::json::LoadException& e) {
                EXPECT_STREQ(e.what(), "Parse /tmp/blet_test_loadFile_except_open_file.json: (Open file failed).");
                EXPECT_EQ(e.message(), "Open file failed");
                EXPECT_EQ(e.filename(), "/tmp/blet_test_loadFile_except_open_file.json");
                EXPECT_EQ(e.line(), 0);
                EXPECT_EQ(e.column(), 0);
                throw;
            }
        },
        blet::json::LoadException);
}

GTEST_TEST(loadFile, except_parsing) {
    // clang-format off
    std::string jsonStr = JSON_TO_STRING((
        {
            "key" : "value".,
            "array": [
                0,1,2,3,4,5,6,7,8,9,10
            ]
        }
    ));
    // clang-format on

    // create example file
    const char* testFile = "/tmp/blet_test_loadFile_except_parsing.json";
    test::blet::FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    fileGuard << jsonStr << std::flush;
    fileGuard.close();

    EXPECT_THROW(
        {
            try {
                blet::Dict json = blet::json::loadFile(testFile);
            }
            catch (const blet::json::LoadException& e) {
                EXPECT_STREQ(e.what(),
                             "Parse at /tmp/blet_test_loadFile_except_parsing.json:1:15 (Key of object not found).");
                EXPECT_EQ(e.message(), "Key of object not found");
                EXPECT_EQ(e.filename(), "/tmp/blet_test_loadFile_except_parsing.json");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 15);
                throw;
            }
        },
        blet::json::LoadException);
}

GTEST_TEST(loadFile, success) {
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
    const char* testFile = "/tmp/blet_test_loadFile_success.json";
    test::blet::FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    fileGuard << jsonStr << std::flush;
    fileGuard.close();

    blet::Dict dict = blet::json::loadFile(testFile);
    EXPECT_EQ(dict.contains("key"), true);
    EXPECT_EQ(dict["key"].getString(), "value");
    EXPECT_EQ(dict.contains("array"), true);
    EXPECT_EQ(dict.at("array").at(0).getNumber(), 0);
    EXPECT_EQ(dict.at("array").at(1).getNumber(), 1);
    EXPECT_EQ(dict.at("array").at(2).getNumber(), 2);
    EXPECT_EQ(dict.at("array").at(3).getNumber(), 3);
    EXPECT_EQ(dict.at("array").at(4).getNumber(), 4);
    EXPECT_EQ(dict.at("array").at(5).getNumber(), 5);
    EXPECT_EQ(dict.at("array").at(6).getNumber(), 6);
    EXPECT_EQ(dict.at("array").at(7).getNumber(), 7);
    EXPECT_EQ(dict.at("array").at(8).getNumber(), 8);
    EXPECT_EQ(dict.at("array").at(9).getNumber(), 9);
    EXPECT_EQ(dict.at("array").at(10).getNumber(), 10);
}
