//
// Created by nguyentd on 3/11/2022.
//
#include "Core/Config/Config.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;

TEST(ConfigTest, ReadMosquitoConfigTest) {
  Config c;
  c.read_from_file("input.yml");

  EXPECT_EQ(c.mosquito_config().daily_report, false);
  EXPECT_EQ(c.mosquito_config().interrupted_feeding_rate, 0.19);
  EXPECT_EQ(c.mosquito_config().cell_size, 5);
  EXPECT_EQ(c.mosquito_config().prmc_size, 800);

}