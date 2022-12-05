#include <gtest/gtest.h>

#include "mblet/jsonator.h"
#include "mock/fileGuard.h"

#define JSON_TO_STRING(...) #__VA_ARGS__

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
    const char str[] = JSON_TO_STRING(
        {
            "key" : "value"
        }
    );
    // clang-format on

    // create example file
    const char* testFile = "/tmp/mblet_test_parsefile_success.json";
    mblet::FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    fileGuard << str << std::flush;
    fileGuard.close();

    mblet::Jsonator json = mblet::Jsonator::parseFile(testFile);
    EXPECT_EQ(json.getFilename(), "/tmp/mblet_test_parsefile_success.json");
    EXPECT_EQ(json.hasKey("key"), true);
    EXPECT_EQ(json.at("key").getString(), "value");
}