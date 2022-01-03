//
// Created by nguyentd on 12/7/2021.
//

#include <gtest/gtest.h>

#include "Core/Config/Config.h"
#include "Parasites/Genotype.h"

class GenotypeTest : public ::testing::Test {
protected:
  void SetUp() override {}

  // void TearDown() override {}
};

TEST_F(GenotypeTest, InitializeWithAASequence) {
  Genotype g("||||NY1||KTHFI,x||||||FNMYRIPRPC|1");

  EXPECT_EQ(g.aa_structure[0].size(), 0);
  EXPECT_EQ(g.aa_structure[4][0], "NY1");
  EXPECT_EQ(g.aa_structure[6][0], "KTHFI");
  EXPECT_EQ(g.aa_structure[6][1], "x");
  EXPECT_EQ(g.aa_structure[12][0], "FNMYRIPRPC");
  EXPECT_EQ(g.aa_structure[13][0], "1");

  EXPECT_EQ(g.get_aa_sequence(), "||||NY1||KTHFI,x||||||FNMYRIPRPC|1");
}

TEST_F(GenotypeTest, CheckValid) {
  Config c;
  c.read_from_file("input.yml");

  Genotype g("||||NY1||KTHFI,x||||||FNMYRIPRPC|1");
  EXPECT_TRUE(g.is_valid(c.pf_gene_info()));

  Genotype g2("||||NY2||KTHFI,x||||||FNMYRIPRPC|1");
  EXPECT_TRUE(g2.is_valid(c.pf_gene_info()));

  Genotype g3("||||NY2||KTHFI,x||||||FNMYRIPRPC|2");
  EXPECT_TRUE(g3.is_valid(c.pf_gene_info()));
}

TEST_F(GenotypeTest, CheckInvalid) {
  Config c;
  c.read_from_file("input.yml");

  Genotype g1("||||NY1||KTHFI||||||FNMYRIPRPC|1");
  EXPECT_FALSE(g1.is_valid(c.pf_gene_info()));

  Genotype g2("|");
  EXPECT_FALSE(g2.is_valid(c.pf_gene_info()));

  Genotype g3("||||MY1||KTHFI,x||||||FNMYRIPRPC|1");
  EXPECT_FALSE(g3.is_valid(c.pf_gene_info()));

  Genotype g4("||||NY3||KTHFI,x||||||FNMYRIPRPC|1");
  EXPECT_FALSE(g4.is_valid(c.pf_gene_info()));

  Genotype g5("||||NYC2||KTHFI,x||||||FNMYRIPRPC|1");
  EXPECT_FALSE(g5.is_valid(c.pf_gene_info()));

  Genotype g6("||||NY2||KTHFI,x||||||FNMYRIPRPC|Y1");
  EXPECT_FALSE(g6.is_valid(c.pf_gene_info()));
}

TEST_F(GenotypeTest, CalculateCostOfResistance) {
  Config c;
  c.read_from_file("input.yml");

  Genotype g("||||NY1||KTHFI,x||||||FNMYRIPRPC|1");
  g.calculate_daily_fitness(c.pf_gene_info());
  EXPECT_EQ(g.daily_fitness_multiple_infection, 1);

  Genotype g1("||||NY1||KTHFI,X||||||FNMYRIPRPC|1");
  g1.calculate_daily_fitness(c.pf_gene_info());
  EXPECT_EQ(g1.daily_fitness_multiple_infection, pow(1 - 0.0005, 1));

  Genotype g2("||||YF2||KTHFI,X||||||FNMYRIPRPC|1");
  g2.calculate_daily_fitness(c.pf_gene_info());
  EXPECT_EQ(g2.daily_fitness_multiple_infection, pow(1 - 0.0005, 3) * (1 - 0.0055));
}