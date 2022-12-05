#include <gtest/gtest.h>

#include "mblet/jsonator.h"

#define JSON_TO_STRING(...) #__VA_ARGS__

// static std::string removeSpace(const std::string& str) {
//     std::string ret("");
//     for (std::size_t i = 0; i < str.size(); ++i) {
//         if (str[i] == '"') {
//             ret.push_back(str[i]);
//             ++i;
//             while (str[i] != '\n' && str[i] != '\0' && str[i] != '"') {
//                 if (str[i] == '\\' && (str[i + 1] == '"' || str[i + 1] == '\\')) {
//                     ret.push_back(str[i]);
//                     ++i; // escape character
//                 }
//                 ret.push_back(str[i]);
//                 ++i;
//             }
//         }
//         if (!::isspace(str[i])) {
//             ret.push_back(str[i]);
//         }
//     }
//     return ret;
// }

GTEST_TEST(jsonator, find) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json.find("foo");
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NONE).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                const mblet::Jsonator json;
                json.find("foo");
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NONE).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json.find(0);
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NONE).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                const mblet::Jsonator json;
                json.find(0);
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NONE).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    {
        mblet::Jsonator json;
        json["foo"] = 42;
        EXPECT_EQ(json.find("foo")->first, "foo");
        EXPECT_EQ(json.find("foo")->second.getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json["foo"] = 42;
        const mblet::Jsonator& cjson = json;
        EXPECT_EQ(cjson.find("foo")->first, "foo");
        EXPECT_EQ(cjson.find("foo")->second.getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json[0] = 42;
        EXPECT_EQ(json.find(0)->first, "0");
        EXPECT_EQ(json.find(0)->second.getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json[0] = 42;
        const mblet::Jsonator& cjson = json;
        EXPECT_EQ(cjson.find(0)->first, "0");
        EXPECT_EQ(cjson.find(0)->second.getNumber(), 42);
    }
}

GTEST_TEST(jsonator, bracket) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                json["foo"];
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NUMBER).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                const mblet::Jsonator& cjson = json;
                cjson["foo"];
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NUMBER).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                json[0];
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NUMBER).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                const mblet::Jsonator& cjson = json;
                cjson[0];
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NUMBER).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json["foo"] = 42;
                const mblet::Jsonator& cjson = json;
                cjson["bar"];
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "has not a child 'bar'.");
                EXPECT_EQ(e.message(), "has not a child");
                EXPECT_EQ(e.child(), "bar");
                throw;
            }
        },
        mblet::Jsonator::ChildException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json[0] = 42;
                const mblet::Jsonator& cjson = json;
                cjson[1];
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "out of range '1'.");
                EXPECT_EQ(e.message(), "out of range");
                EXPECT_EQ(e.index(), 1);
                throw;
            }
        },
        mblet::Jsonator::ChildException);
    {
        mblet::Jsonator json;
        json["foo"] = 42;
        EXPECT_EQ(json.find("foo")->first, "foo");
        EXPECT_EQ(json.find("foo")->second.getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json["foo"] = 42;
        const mblet::Jsonator& cjson = json;
        EXPECT_EQ(cjson.find("foo")->first, "foo");
        EXPECT_EQ(cjson.find("foo")->second.getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json[0] = 42;
        EXPECT_EQ(json.find(0)->first, "0");
        EXPECT_EQ(json.find(0)->second.getNumber(), 42);
    }
    {
        mblet::Jsonator json;
        json[0] = 42;
        const mblet::Jsonator& cjson = json;
        EXPECT_EQ(cjson.find(0)->first, "0");
        EXPECT_EQ(cjson.find(0)->second.getNumber(), 42);
    }
}

GTEST_TEST(jsonator, at) {
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                json.at("foo");
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NUMBER).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                const mblet::Jsonator& cjson = json;
                cjson.at("foo");
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a object (is NUMBER).");
                EXPECT_EQ(e.message(), "is not a object");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                json.at(0);
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NUMBER).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json = 42;
                const mblet::Jsonator& cjson = json;
                cjson.at(0);
            }
            catch (const mblet::Jsonator::AccessException& e) {
                EXPECT_STREQ(e.what(), "is not a array (is NUMBER).");
                EXPECT_EQ(e.message(), "is not a array");
                throw;
            }
        },
        mblet::Jsonator::AccessException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json["foo"] = 42;
                json.at("bar");
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "has not a child 'bar'.");
                EXPECT_EQ(e.message(), "has not a child");
                EXPECT_EQ(e.child(), "bar");
                throw;
            }
        },
        mblet::Jsonator::ChildException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json["foo"] = 42;
                const mblet::Jsonator& cjson = json;
                cjson.at("bar");
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "has not a child 'bar'.");
                EXPECT_EQ(e.message(), "has not a child");
                EXPECT_EQ(e.child(), "bar");
                throw;
            }
        },
        mblet::Jsonator::ChildException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json[0] = 42;
                json.at(1);
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "out of range '1'.");
                EXPECT_EQ(e.message(), "out of range");
                EXPECT_EQ(e.index(), 1);
                throw;
            }
        },
        mblet::Jsonator::ChildException);
    EXPECT_THROW(
        {
            try {
                mblet::Jsonator json;
                json[0] = 42;
                const mblet::Jsonator& cjson = json;
                cjson.at(1);
            }
            catch (const mblet::Jsonator::ChildException& e) {
                EXPECT_STREQ(e.what(), "out of range '1'.");
                EXPECT_EQ(e.message(), "out of range");
                EXPECT_EQ(e.index(), 1);
                throw;
            }
        },
        mblet::Jsonator::ChildException);
}

GTEST_TEST(jsonator, front) {

}

GTEST_TEST(jsonator, back) {

}