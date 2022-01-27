//
// Created by nguyentd on 1/26/2022.
//
#include "Core/Config/Config.h"
#include "Parasites/Genotype.h"
#include "gtest/gtest.h"

TEST(GenotypeOverridEC50Test, OverrideEC50Alone) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_cases = {
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.8, 20) },
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.8, 20) },

  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_cases) {
    Genotype g(gene_str);
    g.override_EC50_power_n(c.override_ec50_patterns(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.00001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}

TEST(GenotypeOverridEC50Test, OverrideEC50AfterCalculate) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, double>> test_cases = {
    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|1", 1, pow(0.8, 20) },

    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.8, 20) },
    { "||||YY1||KTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.67, 20) },
    { "||||NF1||KTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.9, 20) },
    { "||||YF1||KTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.8, 20) },

    { "||||NY2||KTHFI,x||||||FNMYRIPRPC|2", 1, pow(1.0, 20) },
    { "||||YY2||KTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.87, 20) },
    { "||||NF2||KTHFI,x||||||FNMYRIPRPC|2", 1, pow(1.1, 20) },
    { "||||YF2||KTHFI,x||||||FNMYRIPRPC|2", 1, pow(1.0, 20) },

    { "||||NY1||TTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.75, 20) },
    { "||||YY1||TTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.6, 20) },
    { "||||NF1||TTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.85, 20) },
    { "||||YF1||TTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.75, 20) },

    { "||||NY2||TTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.95, 20) },
    { "||||YY2||TTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.8, 20) },
    { "||||NF2||TTHFI,x||||||FNMYRIPRPC|2", 1, pow(1.05, 20) },
    { "||||YF2||TTHFI,x||||||FNMYRIPRPC|2", 1, pow(0.95, 20) },

    { "||||NY1||KTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.62, 19) },
    { "||||YY1||KTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.85, 19) },
    { "||||NF1||KTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.5, 19) },
    { "||||YF1||KTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.775, 19) },

    { "||||NY2||KTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.62, 19) },
    { "||||YY2||KTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.85, 19) },
    { "||||NF2||KTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.5, 19) },
    { "||||YF2||KTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.775, 19) },

    { "||||NY1||TTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.7, 19) },
    { "||||YY1||TTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.9, 19) },
    { "||||NF1||TTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.65, 19) },
    { "||||YF1||TTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.82, 19) },

    { "||||NY2||TTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.7, 19) },
    { "||||YY2||TTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.9, 19) },
    { "||||NF2||TTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.65, 19) },
    { "||||YF2||TTHFI,x||||||FNMYRIPRPC|2", 2, pow(0.82, 19) },

  };

  for (const auto& [gene_str, drug_id, ec50_p_n] : test_cases) {
    Genotype g(gene_str);
    g.calculate_EC50_power_n(c.pf_gene_info(), c.drug_db());
    g.override_EC50_power_n(c.override_ec50_patterns(), c.drug_db());
    EXPECT_NEAR(g.get_EC50_power_n(c.drug_db()->at(drug_id)), ec50_p_n, 0.00001)
        << fmt::format("{}-{}-{}", gene_str, drug_id, ec50_p_n);
  }
}