//
// Created by nguyentd on 1/27/2022.
//
#include "../MockRandom.h"
#include "Core/Config/Config.h"
#include "Parasites/Genotype.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::Return;

TEST(GenotypePerformMutationTest, MutationNormalAAMask1) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, int, int, std::string>> test_cases = {
    {
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
        0,
        9,
        0,
        "||||YY1||TTHFI,x||||||FNMYRIPRPY|1",
    },
    {
        "||||YY1||TTHFI,x||||||FNMYRIPRPY|1",
        0,
        9,
        0,
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
    },
    {
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
        0,
        6,
        0,
        "||||YY1||TTHFI,x||||||FNMYRILRPC|1",
    },
    {
        "||||YY1||TTHFI,x||||||FNMYRILRPC|1",
        0,
        6,
        0,
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
    },
    {
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
        1,
        3,
        0,
        "||||YY1||KTHFI,x||||||FNMYRIPRPC|1",
    },
    {
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
        1,
        1,
        0,
        "||||YF1||TTHFI,x||||||FNMYRIPRPC|1",
    },
    {
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
        1,
        0,
        0,
        "||||NY1||TTHFI,x||||||FNMYRIPRPC|1",
    },
  };

  for (const auto& [original_str, drug_id, res_aa_id, random_aa_id, mutant_str] : test_cases) {
    auto origial_genotype = c.genotype_db.get_genotype(original_str, &c);

    MockRandom random;
    EXPECT_CALL(random, random_uniform(_)).WillOnce(Return(res_aa_id)).WillRepeatedly(Return(random_aa_id));
    auto mutant_genotype = origial_genotype->perform_mutation_by_drug(&c, &random, c.drug_db()->at(drug_id));
    EXPECT_EQ(mutant_genotype->aa_sequence, mutant_str);
  }
}

TEST(GenotypePerformMutationTest, MutationCopyNumberVariation) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, int, int, std::string>> test_cases = {
    {
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
        1,
        2,
        0,
        "||||YY2||TTHFI,x||||||FNMYRIPRPC|1",
    },
    {
        "||||YY2||TTHFI,x||||||FNMYRIPRPC|1",
        1,
        2,
        0,
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
    },
  };

  for (const auto& [original_str, drug_id, res_aa_id, random_aa_id, mutant_str] : test_cases) {
    auto origial_genotype = c.genotype_db.get_genotype(original_str, &c);

    MockRandom random;
    EXPECT_CALL(random, random_uniform(_)).WillOnce(Return(res_aa_id)).WillRepeatedly(Return(random_aa_id));
    auto mutant_genotype = origial_genotype->perform_mutation_by_drug(&c, &random, c.drug_db()->at(drug_id));
    EXPECT_EQ(mutant_genotype->aa_sequence, mutant_str);
  }
}

TEST(GenotypePerformMutationTest, MutationNormalAAMask0) {
  Config c;
  c.read_from_file("input.yml");

  std::vector<std::tuple<std::string, int, int, int, std::string>> test_cases = {
    {
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
        0,
        0,
        0,
        "||||YY1||TTHFI,x||||||FNMYRIPRPC|1",
    },
  };

  for (const auto& [original_str, drug_id, res_aa_id, random_aa_id, mutant_str] : test_cases) {
    auto origial_genotype = c.genotype_db.get_genotype(original_str, &c);

    MockRandom random;
    EXPECT_CALL(random, random_uniform(_)).WillOnce(Return(res_aa_id)).WillRepeatedly(Return(random_aa_id));
    auto mutant_genotype = origial_genotype->perform_mutation_by_drug(&c, &random, c.drug_db()->at(drug_id));
    EXPECT_EQ(mutant_genotype->aa_sequence, mutant_str);
  }
}