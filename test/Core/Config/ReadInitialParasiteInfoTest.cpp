//
// Created by nguyentd on 12/31/2021.
//

#include <gtest/gtest.h>
#include "Core/Config/Config.h"

TEST(ConfigTest, ReadInitialParasiteInfo) {
  Config c;
  c.read_from_file("input.yml");

  ASSERT_EQ(c.initial_parasite_info().size(), 18);
  for (auto i = 0; i < 18; i += 2) {
    ASSERT_EQ(c.initial_parasite_info()[i].parasite_type_id, 0);
    ASSERT_EQ(c.initial_parasite_info()[i].prevalence, 0.05);
  }
  for (auto i = 1; i < 18; i += 2) {
    ASSERT_EQ(c.initial_parasite_info()[i].parasite_type_id, 1);
    ASSERT_EQ(c.initial_parasite_info()[i].prevalence, 0.05);
  }
}
