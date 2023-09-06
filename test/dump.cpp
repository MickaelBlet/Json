#include <gtest/gtest.h>

#include "blet/json.h"
#include "mock/jsonToString.h"

GTEST_TEST(dump, special_character) {
    // clang-format off
    // create example string
    std::string jsonStr = JSON_TO_STRING((
        [
            {
                "\a\b\f\n\r\t\v\'\"\\": null
            }
        ]
    ));
    // clang-format on

    blet::Dict dict = blet::json::loadString(jsonStr);
    EXPECT_EQ(blet::json::dump(dict), "[{\"\\a\\b\\f\\n\\r\\t\\v'\\\"\\\\\":null}]");
}

GTEST_TEST(dump, indent) {
    // clang-format off
    std::string jsonStr = JSON_TO_STRING((
        {
            "root" : {
                "1": [
                    0,
                    1,
                    2
                ],
                "2": true,
                "3": null,
                "4": 42,
                "5": {
                    "0": 0,
                    "1": 1
                },
                "6": "42"
            }
        }
    ));
    // clang-format on

    blet::Dict dict = blet::json::loadString(jsonStr);

    const char result[] =
        "{\n"
        "  \"root\": {\n"
        "    \"1\": [\n"
        "      0,\n"
        "      1,\n"
        "      2\n"
        "    ],\n"
        "    \"2\": true,\n"
        "    \"3\": null,\n"
        "    \"4\": 42,\n"
        "    \"5\": {\n"
        "      \"0\": 0,\n"
        "      \"1\": 1\n"
        "    },\n"
        "    \"6\": \"42\"\n"
        "  }\n"
        "}";

    EXPECT_EQ(blet::json::dump(dict, 2), result);
}