//
// Created by nguyentd on 1/26/2022.
//
#include <gtest/gtest.h>

#include "Core/Config/Config.h"

TEST(ConfigUpdateMutationMaskTest, ConfigUpdateMutationMaskTest) {
  Config c;
  c.read_from_file("input.yml");
//  for (const auto &aa_loc : c.drug_db()->at(1)->resistant_aa_locations) {
//    std::cout << aa_loc.chromosome_id << " - " << aa_loc.gene_id << " - " << aa_loc.aa_id << std::endl;
//  }

  ASSERT_EQ(c.drug_db()->at(1)->resistant_aa_locations.size(), 4);
  ASSERT_EQ(c.mutation_mask(), "||||111||11111,0||||||0000001001|1");

  c.update_mutation_mask("||||001||10000,0||||||0000000001|1");

  ASSERT_EQ(c.mutation_mask(), "||||001||10000,0||||||0000000001|1");

  ASSERT_EQ(c.drug_db()->at(0)->resistant_aa_locations.size(), 1);
  ASSERT_EQ(c.drug_db()->at(1)->resistant_aa_locations.size(), 2);
}