//StringHelpersTest

#include "Helpers/TimeHelpers.h"
#include "date/date.h"
#include "Helpers/StringHelpers.h"
#include "gtest/gtest.h"

using namespace date::literals;

TEST(StringHelpers, SplitString){
    const std::string input = "a b c d e";
    const auto result = StringHelpers::split(input, ' ');
    ASSERT_EQ(5, result.size());

    std::vector<std::string> expected{"a", "b", "c", "d", "e"};
    for (auto i = 0; i < 5; ++i) {
    ASSERT_EQ(result[i], expected[i]);
    }
}

TEST(StringHelpers, SplitStringWithSpaces){
    const std::string input = "a  b   c    d   e";
    const auto result = StringHelpers::split(input, ' ');
    ASSERT_EQ(5, result.size());

    std::vector<std::string> expected{"a", "b", "c", "d", "e"};
    for (auto i = 0; i < 5; ++i) {
        ASSERT_EQ(result[i], expected[i]);
    }
}

