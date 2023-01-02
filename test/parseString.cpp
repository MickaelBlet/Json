#include <gtest/gtest.h>

#include "mblet/jsonator.h"
#include "mock/jsonToString.h"

GTEST_TEST(parseString, comment) {
    // clang-format off
    const char jsonStr[]= \
        "{\n" \
        "    // comment line\n" \
        "    /* comment\nblock */\n" \
        "}";
    // clang-format on

    mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
    EXPECT_EQ(json.size(), 0);
}

GTEST_TEST(parseString, empty) {
    mblet::Jsonator json = mblet::Jsonator::parseString("");
    EXPECT_EQ(json.size(), 0);
    json = json.parseString("  ");
    EXPECT_EQ(json.size(), 0);
    json = json.parseString("\t");
    EXPECT_EQ(json.size(), 0);
    json = json.parseString("\n");
    EXPECT_EQ(json.size(), 0);
}

GTEST_TEST(parseString, Not_a_valid_start_character) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json = mblet::Jsonator::parseString("Oo");
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:1 (Not a valid start character).");
                EXPECT_EQ(e.message(), "Not a valid start character");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 1);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json = mblet::Jsonator::parseString("\tOo");
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:2 (Not a valid start character).");
                EXPECT_EQ(e.message(), "Not a valid start character");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 2);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json = mblet::Jsonator::parseString("   Oo   ");
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:4 (Not a valid start character).");
                EXPECT_EQ(e.message(), "Not a valid start character");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 4);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, Not_a_valid_end_character) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json = mblet::Jsonator::parseString("{})");
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:3 (Not a valid end character).");
                EXPECT_EQ(e.message(), "Not a valid end character");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 3);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json = mblet::Jsonator::parseString("{}-");
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:3 (Not a valid end character).");
                EXPECT_EQ(e.message(), "Not a valid end character");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 3);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json = mblet::Jsonator::parseString("{}}");
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:3 (Not a valid end character).");
                EXPECT_EQ(e.message(), "Not a valid end character");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 3);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, parseType_False) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = JSON_TO_STRING((
                    [
                        ttrue // not true
                    ]
                ));
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:2 (Bad element of array).");
                EXPECT_EQ(e.message(), "Bad element of array");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 2);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = JSON_TO_STRING((
                    [
                        ffalse // not false
                    ]
                ));
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:2 (Bad element of array).");
                EXPECT_EQ(e.message(), "Bad element of array");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 2);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = JSON_TO_STRING((
                    [
                        nnull // not null
                    ]
                ));
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:2 (Bad element of array).");
                EXPECT_EQ(e.message(), "Bad element of array");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 2);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = JSON_TO_STRING((
                    [
                        abracadabra // default
                    ]
                ));
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:2 (Bad element of array).");
                EXPECT_EQ(e.message(), "Bad element of array");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 2);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, End_of_object_not_found) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "{";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:2 (End of object not found).");
                EXPECT_EQ(e.message(), "End of object not found");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 2);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, Bad_element_in_the_key) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = JSON_TO_STRING((
                    {
                        "key": abracadabra
                    }
                ));
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:8 (Bad element in the key).");
                EXPECT_EQ(e.message(), "Bad element in the key");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 8);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, Key_of_object_not_found) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = JSON_TO_STRING((
                    {
                        {}
                    }
                ));
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:2 (Key of object not found).");
                EXPECT_EQ(e.message(), "Key of object not found");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 2);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, End_of_array_not_found) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "[";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:2 (End of array not found).");
                EXPECT_EQ(e.message(), "End of array not found");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 2);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, End_of_key) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                const char jsonStr[]="{\"";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:3 (End of key).");
                EXPECT_EQ(e.message(), "End of key");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 3);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, New_line_in_key) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                const char jsonStr[]="{\"\n\"";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:3 (New line in key).");
                EXPECT_EQ(e.message(), "New line in key");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 3);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, Need_definition_of_object) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                const char jsonStr[]="{\"key\"}";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:7 (Need definition of object).");
                EXPECT_EQ(e.message(), "Need definition of object");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 7);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, Key_already_exist) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                const char jsonStr[]="{\"key\": null,\"key\": null}";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:15 (Key already exist).");
                EXPECT_EQ(e.message(), "Key already exist");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 15);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, End_of_string) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                const char jsonStr[]="[\"";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:3 (End of string).");
                EXPECT_EQ(e.message(), "End of string");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 3);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, New_line_in_string) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                const char jsonStr[]="[\"\n\"]";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:3 (New line in string).");
                EXPECT_EQ(e.message(), "New line in string");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 3);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, Octal_number_not_allowed) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                const char jsonStr[]="[076]";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:2 (Octal number not allowed).");
                EXPECT_EQ(e.message(), "Octal number not allowed");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 2);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}

GTEST_TEST(parseString, Bad_number) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                const char jsonStr[]="[ - ]";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:3 (Bad number).");
                EXPECT_EQ(e.message(), "Bad number");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 3);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
    EXPECT_THROW(
        {
            try {
                // clang-format off
                const char jsonStr[]="[ 42toto ]";
                // clang-format on
                mblet::Jsonator json = mblet::Jsonator::parseString(jsonStr);
            }
            catch (const mblet::Jsonator::ParseException& e) {
                EXPECT_STREQ(e.what(), "Parse at 1:5 (Bad element of array).");
                EXPECT_EQ(e.message(), "Bad element of array");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 5);
                throw;
            }
        },
        mblet::Jsonator::ParseException);
}