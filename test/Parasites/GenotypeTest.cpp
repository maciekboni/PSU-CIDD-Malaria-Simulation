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
  EXPECT_TRUE(g.is_valid(c.pf_genotype_info()));

  Genotype g2("||||NY2||KTHFI,x||||||FNMYRIPRPC|1");
  EXPECT_TRUE(g2.is_valid(c.pf_genotype_info()));

  Genotype g3("||||NY2||KTHFI,x||||||FNMYRIPRPC|2");
  EXPECT_TRUE(g3.is_valid(c.pf_genotype_info()));
}

TEST_F(GenotypeTest, CheckInvalid) {
  Config c;
  c.read_from_file("input.yml");

  Genotype g1("||||NY1||KTHFI||||||FNMYRIPRPC|1");
  EXPECT_FALSE(g1.is_valid(c.pf_genotype_info()));

  Genotype g2("|");
  EXPECT_FALSE(g2.is_valid(c.pf_genotype_info()));

  Genotype g3("||||MY1||KTHFI,x||||||FNMYRIPRPC|1");
  EXPECT_FALSE(g3.is_valid(c.pf_genotype_info()));

  Genotype g4("||||NY3||KTHFI,x||||||FNMYRIPRPC|1");
  EXPECT_FALSE(g4.is_valid(c.pf_genotype_info()));

  Genotype g5("||||NYC2||KTHFI,x||||||FNMYRIPRPC|1");
  EXPECT_FALSE(g5.is_valid(c.pf_genotype_info()));

  Genotype g6("||||NY2||KTHFI,x||||||FNMYRIPRPC|Y1");
  EXPECT_FALSE(g6.is_valid(c.pf_genotype_info()));
}

TEST_F(GenotypeTest, CalculateCostOfResistance) {
  Config c;
  c.read_from_file("input.yml");

  Genotype g("||||NY1||KTHFI,x||||||FNMYRIPRPC|1");
  g.calculate_daily_fitness(c.pf_genotype_info());
  EXPECT_DOUBLE_EQ(g.daily_fitness_multiple_infection, 1);

  Genotype g1("||||NY1||KTHFI,X||||||FNMYRIPRPC|1");
  g1.calculate_daily_fitness(c.pf_genotype_info());
  EXPECT_DOUBLE_EQ(g1.daily_fitness_multiple_infection, pow(1 - 0.0005, 1));

  Genotype g2("||||YF2||KTHFI,X||||||FNMYRIPRPC|1");
  g2.calculate_daily_fitness(c.pf_genotype_info());
  EXPECT_DOUBLE_EQ(g2.daily_fitness_multiple_infection, pow(1 - 0.0005, 3) * (1 - 0.0055));
}

TEST_F(GenotypeTest, Calculate_Base_EC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_samples = {
    // base ec50
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 0, pow(0.75, 25) },
    { "||||YY1||TTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.6, 20) },
    { "||||NF1||KTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.5, 19) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 3, pow(0.58, 15) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 4, pow(0.45, 15) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 5, pow(1.08, 15) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 6, pow(0.72, 19) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 7, pow(0.55, 15) }
  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_samples) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_genotype_info(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.0000000001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}

TEST_F(GenotypeTest, Calculate_AS_EC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_cases = {
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

    // triple mutation
    { "||||NY1||KTHFI,x||||||FYMYRIPHPY|1", 0, pow(1.452, 25) },

    // triple mutation on k13 and none relevant genes
    { "||||YF2||KTHFI,x||||||FYMYRIPHPY|1", 0, pow(1.452, 25) },
    { "||||YF2||KTHFI,X||||||FYMYRIPHPY|2", 0, pow(1.452, 25) },

    // quadruple mutation on k13 and none relevant genes
    { "||||YF2||KTHFI,X||||||IYMYRIPHPY|2", 0, pow(1.5972, 25) },

  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_cases) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_genotype_info(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.000001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}

TEST_F(GenotypeTest, Calculate_Piperaquine_EC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_cases = {
    // base ec50
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 3, pow(0.58, 15) },
    // mutation on none relevant genes
    { "||||YF2||TTHFI,X||||||FNMYRIPRPY|1", 3, pow(0.58, 15) },

    // copy number variation
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|2", 3, pow(1.4, 15) },

    // copy number variation and none relevant genes
    { "||||NY2||KTHFI,x||||||FNMYRIPRPY|2", 3, pow(1.4, 15) },

  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_cases) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_genotype_info(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.00001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}

TEST_F(GenotypeTest, Calculate_Mefloquine_EC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_cases = {
    // base ec50
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 4, pow(0.45, 15) },
    // mutation on none relevant genes
    { "||||YF1||KTHFI,x||||||FNMYRIPRPC|1", 4, pow(0.45, 15) },

    // copy number variation
    { "||||NY2||KTHFI,x||||||FNMYRIPRPC|1", 4, pow(1.1, 15) },

    // copy number variation and none relevant genes
    { "||||YF2||TTHFI,X||||||FNMYRIPHPY|1", 4, pow(1.1, 15) },

  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_cases) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_genotype_info(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.00001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}

TEST_F(GenotypeTest, Calculate_SP_EC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_cases = {
    // base ec50
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 5, pow(1.08, 15) },
    // mutations has no effect
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|2", 5, pow(1.08, 15) },
    { "||||YF2||TTHFI,X||||||FNMYRIPHPY|1", 5, pow(1.08, 15) },

  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_cases) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_genotype_info(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.00001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}

TEST_F(GenotypeTest, Calculate_CQ_EC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_cases = {
    // base ec50
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 6, pow(0.72, 19) },

    // only N85Y in mdr1 has effect
    { "||||NF1||KTHFI,x||||||FNMYRIPRPC|1", 6, pow(0.72, 19) },
    { "||||NY2||KTHFI,x||||||FNMYRIPRPC|1", 6, pow(0.72, 19) },
    { "||||NF2||KTHFI,x||||||FNMYRIPRPC|1", 6, pow(0.72, 19) },
    { "||||YY1||KTHFI,x||||||FNMYRIPRPC|1", 6, pow(0.9, 19) },
    { "||||YF1||KTHFI,x||||||FNMYRIPRPC|1", 6, pow(0.9, 19) },
    { "||||YF2||KTHFI,x||||||FNMYRIPRPC|1", 6, pow(0.9, 19) },

    // K76T
    { "||||NY1||TTHFI,x||||||FNMYRIPRPC|1", 6, pow(1.152, 19) },
    { "||||NY1||TSHFI,x||||||FNMYRIPRPY|2", 6, pow(1.152, 19) },
    { "||||NY2||TTHFI,x||||||FNMYRIPRPC|1", 6, pow(1.152, 19) },
    { "||||NF1||TSHFI,x||||||FNMYRIPRPY|2", 6, pow(1.152, 19) },
    { "||||NF2||TSHFI,x||||||FNMYRIPRPY|2", 6, pow(1.152, 19) },

    // 85Y + 76T
    { "||||YY1||TTHFI,x||||||FNMYRIPRPC|1", 6, pow(1.44, 19) },
    { "||||YF1||TTHFI,x||||||FNMYRIPRPC|1", 6, pow(1.44, 19) },
    { "||||YY2||TTHFI,x||||||FNMYRIPRPC|1", 6, pow(1.44, 19) },
    { "||||YF2||TTHFI,x||||||FNMYRIPRPC|1", 6, pow(1.44, 19) },

    //    1.19 # TODO: test this value
  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_cases) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_genotype_info(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.00001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}

TEST_F(GenotypeTest, Calculate_Lumefantrine_EC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_cases = {
    // base ec50
    { "||||YY1||TTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.6, 20) },

    // mutation on non-relevant genes
    { "||||YY1||TTHFI,X||||||FNMYRIPRPY|2", 1, pow(0.6, 20) },

    // single mutation N86
    { "||||NY1||TTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.75, 20) },
    { "||||NY1||TTHFI,X||||||FNMYRIPRPY|2", 1, pow(0.75, 20) },

    // single mutation 184F
    { "||||YF1||TTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.75, 20) },
    { "||||YF1||TTHFI,X||||||FNMYRIPRPY|2", 1, pow(0.75, 20) },

    // single mutation K76
    { "||||YY1||KTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.66, 20) },
    { "||||YY1||KTHFI,X||||||FNMYRIPRPY|2", 1, pow(0.66, 20) },

    // double copy YYYY + 76T
    { "||||YY2||TTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.78, 20) },

    // double copy YYYY + K76
    { "||||YY2||KTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.858, 20) },

    // N86 Y184 K76
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.825, 20) },
    { "||||NY1||KTHFI,X||||||FNMYRIPRPY|2", 1, pow(0.825, 20) },

    // NYNY - K76
    { "||||NY2||KTHFI,X||||||FNMYRIPRPY|2", 1, pow(1.0725, 20) },

    // N86 - 184F 76T
    { "||||NF1||TTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.7875, 20) },

    // NFNF 76T
    { "||||NF2||TTHFI,x||||||FNMYRIPRPC|1", 1, pow(1.02375, 20) },

    // NF-NF K76
    { "||||NF1||KTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.86625, 20) },
    { "||||NF2||KTHFI,x||||||FNMYRIPRPC|1", 1, pow(1.126125, 20) },

  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_cases) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_genotype_info(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.00001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}

TEST_F(GenotypeTest, Calculate_Amodiaquine_EC50) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_cases = {
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.6, 19) },
    { "||||YY1||KTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.852, 19) },
    { "||||NF1||KTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.5, 19) },
    { "||||YF1||KTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.71, 19) },

    { "||||NY2||KTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.6, 19) },
    { "||||YY2||KTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.852, 19) },
    { "||||NF2||KTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.5, 19) },
    { "||||YF2||KTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.71, 19) },

    { "||||NY1||TTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.72, 19) },
    { "||||YY1||TTHFI,x||||||FNMYRIPRPC|1", 2, pow(1.0224, 19) },
    { "||||NF1||TTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.6, 19) },
    { "||||YF1||TTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.852, 19) },

    { "||||NY2||TTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.72, 19) },
    { "||||YY2||TTHFI,x||||||FNMYRIPRPC|1", 2, pow(1.0224, 19) },
    { "||||NF2||TTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.6, 19) },
    { "||||YF2||TTHFI,x||||||FNMYRIPRPC|1", 2, pow(0.852, 19) },

  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_cases) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_genotype_info(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.00001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}
