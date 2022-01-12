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

  EXPECT_EQ(c.pf_gene_info().chromosome_infos[6].gene_infos.size(), 2);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[4].gene_infos.size(), 1);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[12].gene_infos.size(), 1);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[13].gene_infos.size(), 1);

  EXPECT_EQ(c.pf_gene_info().chromosome_infos[6].gene_infos[0].chromosome, 7);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[4].gene_infos[0].chromosome, 5);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[12].gene_infos[0].chromosome, 13);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[13].gene_infos[0].chromosome, 14);

  EXPECT_EQ(c.pf_gene_info().chromosome_infos[6].gene_infos[0].max_copies, 1);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[4].gene_infos[0].max_copies, 2);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[12].gene_infos[0].max_copies, 1);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[13].gene_infos[0].max_copies, 2);

  EXPECT_EQ(c.pf_gene_info().chromosome_infos[6].gene_infos[0].cnv_daily_crs.size(), 0);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[4].gene_infos[0].cnv_daily_crs.size(), 2);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[12].gene_infos[0].cnv_daily_crs.size(), 0);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[13].gene_infos[0].cnv_daily_crs.size(), 2);

  ASSERT_THAT(c.pf_gene_info().chromosome_infos[4].gene_infos[0].cnv_daily_crs, ElementsAre(0, 0.0055));
  ASSERT_THAT(c.pf_gene_info().chromosome_infos[13].gene_infos[0].cnv_daily_crs, ElementsAre(0, 0.0005));
}

TEST(ConfigTest, ReadAAPositionsTest) {
  Config c;
  c.read_from_file("input.yml");

  EXPECT_EQ(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos.size(), 5);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[4].gene_infos[0].aa_position_infos.size(), 2);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[12].gene_infos[0].aa_position_infos.size(), 10);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[13].gene_infos[0].aa_position_infos.size(), 0);

  EXPECT_EQ(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[0].position, 76);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[1].position, 93);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[2].position, 97);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[3].position, 145);
  EXPECT_EQ(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[4].position, 218);

  ASSERT_THAT(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[0].amino_acids,
              ElementsAre('K', 'T'));
  ASSERT_THAT(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[1].amino_acids,
              ElementsAre('T', 'S'));
  ASSERT_THAT(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[2].amino_acids,
              ElementsAre('H', 'T'));
  ASSERT_THAT(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[3].amino_acids,
              ElementsAre('F', 'I'));
  ASSERT_THAT(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[4].amino_acids,
              ElementsAre('I', 'F'));

  ASSERT_THAT(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[0].daily_crs,
              ElementsAre(0, 0.0005));
  ASSERT_THAT(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[1].daily_crs,
              ElementsAre(0, 0.0005));
  ASSERT_THAT(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[2].daily_crs,
              ElementsAre(0, 0.0005));
  ASSERT_THAT(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[3].daily_crs,
              ElementsAre(0, 0.0005));
  ASSERT_THAT(c.pf_gene_info().chromosome_infos[6].gene_infos[0].aa_position_infos[4].daily_crs,
              ElementsAre(0, 0.0005));
}

TEST(ConfigTest, ReadEC50Test) {
  Config c;
  c.read_from_file("input.yml");

  auto plasmepsin_info = c.pf_gene_info().chromosome_infos[13].gene_infos[0];
  EXPECT_EQ(plasmepsin_info.cnv_multiplicative_effect_on_EC50.size(), 1);

  EXPECT_EQ(plasmepsin_info.cnv_multiplicative_effect_on_EC50[0].size(), 2);
  ASSERT_THAT(plasmepsin_info.cnv_multiplicative_effect_on_EC50[0], ElementsAre(1.0, 1.6));

  auto mdr1_info = c.pf_gene_info().chromosome_infos[4].gene_infos[0];
  EXPECT_EQ(mdr1_info.cnv_multiplicative_effect_on_EC50.size(), 1);
  EXPECT_EQ(mdr1_info.multiplicative_effect_on_EC50_for_2_or_more_mutations, 1.1);

  EXPECT_EQ(mdr1_info.cnv_multiplicative_effect_on_EC50[0].size(), 2);
  ASSERT_THAT(mdr1_info.cnv_multiplicative_effect_on_EC50[0], ElementsAre(1.0, 1.6));

  ASSERT_THAT(mdr1_info.aa_position_infos[0].multiplicative_effect_on_EC50[0], ElementsAre(1.0, 1.6));
}