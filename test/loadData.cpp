#include <gtest/gtest.h>

#include "blet/json.h"
#include "mock/jsonToString.h"

GTEST_TEST(loadData, empty) {
    blet::Dict dict = blet::json::loadData("{}", 2);
    EXPECT_TRUE(dict.empty());
}
