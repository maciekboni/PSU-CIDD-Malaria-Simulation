//
// Created by nguyentd on 12/30/2021.
//
#include "Core/Config/Config.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;

TEST(ConfigTest, ReadGeneTest) {
  Config c;
  c.read_from_file("input.yml");

  EXPECT_EQ(c.gene_info().genes.size(), 4);

  EXPECT_EQ(c.gene_info().genes[0].chromosome, 7);
  EXPECT_EQ(c.gene_info().genes[1].chromosome, 5);
  EXPECT_EQ(c.gene_info().genes[2].chromosome, 13);
  EXPECT_EQ(c.gene_info().genes[3].chromosome, 14);

  EXPECT_EQ(c.gene_info().genes[0].max_copy, 1);
  EXPECT_EQ(c.gene_info().genes[1].max_copy, 2);
  EXPECT_EQ(c.gene_info().genes[2].max_copy, 1);
  EXPECT_EQ(c.gene_info().genes[3].max_copy, 2);

  EXPECT_EQ(c.gene_info().genes[0].copy_daily_crs.size(), 0);
  EXPECT_EQ(c.gene_info().genes[1].copy_daily_crs.size(), 2);
  EXPECT_EQ(c.gene_info().genes[2].copy_daily_crs.size(), 0);
  EXPECT_EQ(c.gene_info().genes[3].copy_daily_crs.size(), 2);

  ASSERT_THAT(c.gene_info().genes[1].copy_daily_crs, ElementsAre(0, 0.0055));
  ASSERT_THAT(c.gene_info().genes[3].copy_daily_crs, ElementsAre(0, 0.0005));

}

TEST(ConfigTest, ReadAAPositionsTest) {
  Config c;
  c.read_from_file("input.yml");

  EXPECT_EQ(c.gene_info().genes[0].aa_positions.size(), 5);
  EXPECT_EQ(c.gene_info().genes[1].aa_positions.size(), 2);
  EXPECT_EQ(c.gene_info().genes[2].aa_positions.size(), 10);
  EXPECT_EQ(c.gene_info().genes[3].aa_positions.size(), 0);

  EXPECT_EQ(c.gene_info().genes[0].aa_positions[0].position, 76);
  EXPECT_EQ(c.gene_info().genes[0].aa_positions[1].position, 93);
  EXPECT_EQ(c.gene_info().genes[0].aa_positions[2].position, 97);
  EXPECT_EQ(c.gene_info().genes[0].aa_positions[3].position, 145);
  EXPECT_EQ(c.gene_info().genes[0].aa_positions[4].position, 218);

  ASSERT_THAT(c.gene_info().genes[0].aa_positions[0].amino_acids, ElementsAre('K', 'T'));
  ASSERT_THAT(c.gene_info().genes[0].aa_positions[1].amino_acids, ElementsAre('T', 'S'));
  ASSERT_THAT(c.gene_info().genes[0].aa_positions[2].amino_acids, ElementsAre('H', 'T'));
  ASSERT_THAT(c.gene_info().genes[0].aa_positions[3].amino_acids, ElementsAre('F', 'I'));
  ASSERT_THAT(c.gene_info().genes[0].aa_positions[4].amino_acids, ElementsAre('I', 'F'));

  ASSERT_THAT(c.gene_info().genes[0].aa_positions[0].daily_crs, ElementsAre(0, 0.0005));
  ASSERT_THAT(c.gene_info().genes[0].aa_positions[1].daily_crs, ElementsAre(0, 0.0005));
  ASSERT_THAT(c.gene_info().genes[0].aa_positions[2].daily_crs, ElementsAre(0, 0.0005));
  ASSERT_THAT(c.gene_info().genes[0].aa_positions[3].daily_crs, ElementsAre(0, 0.0005));
  ASSERT_THAT(c.gene_info().genes[0].aa_positions[4].daily_crs, ElementsAre(0, 0.0005));
}