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

  EXPECT_EQ(g.pf_genotype_str[0].size(), 0);
  EXPECT_EQ(g.pf_genotype_str[4][0], "NY1");
  EXPECT_EQ(g.pf_genotype_str[6][0], "KTHFI");
  EXPECT_EQ(g.pf_genotype_str[6][1], "x");
  EXPECT_EQ(g.pf_genotype_str[12][0], "FNMYRIPRPC");
  EXPECT_EQ(g.pf_genotype_str[13][0], "1");

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
  EXPECT_DOUBLE_EQ(g.daily_fitness_multiple_infection, 1);

  Genotype g1("||||NY1||KTHFI,X||||||FNMYRIPRPC|1");
  g1.calculate_daily_fitness(c.pf_gene_info());
  EXPECT_DOUBLE_EQ(g1.daily_fitness_multiple_infection, pow(1 - 0.0005, 1));

  Genotype g2("||||YF2||KTHFI,X||||||FNMYRIPRPC|1");
  g2.calculate_daily_fitness(c.pf_gene_info());
  EXPECT_DOUBLE_EQ(g2.daily_fitness_multiple_infection, pow(1 - 0.0005, 3) * (1 - 0.0055));
}

TEST_F(GenotypeTest, Calculate_Base_EC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_samples = {
    // base ec50
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 0, pow(0.75, 25) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.6, 20) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.5, 19) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 3, pow(0.58, 15) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 4, pow(0.45, 15) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 5, pow(1.08, 15) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 6, pow(0.72, 19) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 7, pow(0.55, 15) }
  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_samples) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_gene_info(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.0000000001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}

TEST_F(GenotypeTest, Calculate_AS_EC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_samples = {
    // base ec50
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 0, pow(0.75, 25) },
    // single as mutation
    { "||||NY1||KTHFI,x||||||INMYRIPRPC|1", 0, pow(1.2, 25) },
    { "||||NY1||KTHFI,x||||||FYMYRIPRPC|1", 0, pow(1.2, 25) },
    { "||||NY1||KTHFI,x||||||FNIYRIPRPC|1", 0, pow(1.2, 25) },
    { "||||NY1||KTHFI,x||||||FNMHRIPRPC|1", 0, pow(1.2, 25) },
    { "||||NY1||KTHFI,x||||||FNMYTIPRPC|1", 0, pow(1.2, 25) },
    { "||||NY1||KTHFI,x||||||FNMYRTPRPC|1", 0, pow(1.2, 25) },
    { "||||NY1||KTHFI,x||||||FNMYRILRPC|1", 0, pow(1.2, 25) },
    { "||||NY1||KTHFI,x||||||FNMYRIPHPC|1", 0, pow(1.2, 25) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRLC|1", 0, pow(1.2, 25) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPY|1", 0, pow(1.2, 25) },

    // double mutation
    { "||||NY1||KTHFI,x||||||INMYRIPRPY|1", 0, pow(1.32, 25) },
    { "||||NY1||KTHFI,x||||||FYMYRIPRPY|1", 0, pow(1.32, 25) },

    // tripple mutation
    { "||||NY1||KTHFI,x||||||FYMYRIPHPY|1", 0, pow(1.452, 25) },
  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_samples) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_gene_info(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.0000000001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}