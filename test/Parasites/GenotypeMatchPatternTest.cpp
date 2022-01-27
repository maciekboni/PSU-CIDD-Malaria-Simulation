//
// Created by nguyentd on 1/26/2022.
//

#include <gtest/gtest.h>

#include "Core/Config/Config.h"

TEST(GenotypeMatchPatternTest, MatchALPatternTest) {
  std::string pattern = "||||NY1||K....,.||||||..........|.";

  std::vector<std::tuple<std::string, bool>> test_cases = {
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", true },  { "||||NY1||KTHFI,x||||||FNMYRIPHPY|1", true },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|2", true },  { "||||NY1||KNHFI,x||||||FNMYRIPRPC|2", true },

    { "||||NY2||KTHFI,x||||||FNMYRIPRPC|1", false }, { "||||NY1||TTHFI,x||||||FNMYRIPRPC|1", false },
    { "||||NY1||TNHFI,x||||||FNMYRIPRPC|2", false },

  };

  for (const auto& [aa_str, matched] : test_cases) {
    Genotype g(aa_str);
    EXPECT_EQ(g.match_pattern(pattern), matched);
  }
}

TEST(GenotypeMatchPatternTest, MatchAQPatternTest) {
  std::string pattern = "||||NY.||K....,.||||||..........|.";

  std::vector<std::tuple<std::string, bool>> test_cases = {
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|2", true },
    { "||||NY2||KTHFI,x||||||FNMYRIPRPC|2", true },
  };

  for (const auto& [aa_str, matched] : test_cases) {
    Genotype g(aa_str);
    EXPECT_EQ(g.match_pattern(pattern), matched);
  }
}
