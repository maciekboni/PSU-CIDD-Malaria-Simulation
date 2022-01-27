//
// Created by nguyentd on 1/26/2022.
//
#include <gtest/gtest.h>

#include "Core/Config/Config.h"

TEST(ConfigTest, ReadOverrideEC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_cases = {
    { "||||NY1||K....,.||||||..........|.", 1, 0.8 },

  };

  for (const auto& [pattern, drug_id, ec50] : test_cases) {
    EXPECT_EQ(pattern, c.override_ec50_patterns()[0].pattern);
    EXPECT_EQ(drug_id, c.override_ec50_patterns()[0].drug_id);
    EXPECT_EQ(ec50, c.override_ec50_patterns()[0].ec50);
  }
}