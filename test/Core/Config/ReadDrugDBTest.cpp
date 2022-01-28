//
// Created by nguyentd on 1/7/2022.
//
#include "Core/Config/Config.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;

TEST(ConfigTest, ReadCorrectNumberOfDrugs) {
  Config c;
  c.read_from_file("input.yml");
  ASSERT_EQ(c.drug_db()->size(), 8);
  ASSERT_EQ(c.drug_db()->at(0)->n(), 25);
  ASSERT_EQ(c.drug_db()->at(6)->n(), 19);
}

TEST(ConfigTest, ReadBaseEC50) {
  Config c;
  c.read_from_file("input.yml");
  ASSERT_EQ(c.drug_db()->at(0)->base_EC50, 0.75);
  ASSERT_EQ(c.drug_db()->at(1)->base_EC50, 0.6);
  ASSERT_EQ(c.drug_db()->at(2)->base_EC50, 0.5);
  ASSERT_EQ(c.drug_db()->at(7)->base_EC50, 0.55);
}

TEST(ConfigTest, ReadArtemisininResistantAALocation) {
  Config c;
  c.read_from_file("input.yml");
  ASSERT_EQ(c.drug_db()->at(0)->resistant_aa_locations.size(), 10);
  ASSERT_EQ(c.drug_db()->at(0)->resistant_aa_locations[0].is_copy_number, false);

  auto chromosome_id = c.drug_db()->at(0)->resistant_aa_locations[0].chromosome_id;
  auto gene_id = c.drug_db()->at(0)->resistant_aa_locations[0].gene_id;
  ASSERT_EQ(chromosome_id, 12);
  ASSERT_EQ(gene_id, 0);
  ASSERT_EQ(c.pf_gene_info().chromosome_infos[chromosome_id].gene_infos[gene_id].name, "Pfkelch13");
}

TEST(ConfigTest, ReadLumefantrineResistantAALocation) {
  Config c;
  c.read_from_file("input.yml");
  ASSERT_EQ(c.drug_db()->at(1)->resistant_aa_locations.size(), 4);

  auto chromosome_id1 = c.drug_db()->at(1)->resistant_aa_locations[0].chromosome_id;
  auto gene_id1 = c.drug_db()->at(1)->resistant_aa_locations[0].gene_id;
  auto chromosome_id2 = c.drug_db()->at(1)->resistant_aa_locations[3].chromosome_id;
  auto gene_id2 = c.drug_db()->at(1)->resistant_aa_locations[3].gene_id;

  ASSERT_EQ(chromosome_id1, 4);
  ASSERT_EQ(chromosome_id2, 6);
  ASSERT_EQ(gene_id1, 0);
  ASSERT_EQ(gene_id2, 0);
  ASSERT_EQ(c.pf_gene_info().chromosome_infos[chromosome_id1].gene_infos[gene_id1].name, "Pfmdr1");
  ASSERT_EQ(c.pf_gene_info().chromosome_infos[chromosome_id2].gene_infos[gene_id2].name, "Pfcrt");
}

TEST(ConfigTest, ReadKAFResistantAALocation) {
  Config c;
  c.read_from_file("input.yml");
  ASSERT_EQ(c.drug_db()->at(7)->resistant_aa_locations.size(), 1);

  auto chromosome_id = c.drug_db()->at(7)->resistant_aa_locations[0].chromosome_id;
  auto gene_id = c.drug_db()->at(7)->resistant_aa_locations[0].gene_id;
  ASSERT_EQ(chromosome_id, 6);
  ASSERT_EQ(gene_id, 1);
  ASSERT_EQ(c.pf_gene_info().chromosome_infos[chromosome_id].gene_infos[gene_id].name, "Pfkaf");
}

TEST(ConfigTest, ReadPiperaquineResistantAALocation) {
  Config c;
  c.read_from_file("input.yml");
  ASSERT_EQ(c.drug_db()->at(3)->resistant_aa_locations.size(), 1);

  auto chromosome_id = c.drug_db()->at(3)->resistant_aa_locations[0].chromosome_id;
  auto gene_id = c.drug_db()->at(3)->resistant_aa_locations[0].gene_id;
  ASSERT_EQ(chromosome_id, 13);
  ASSERT_EQ(gene_id, 0);
  ASSERT_EQ(c.drug_db()->at(3)->resistant_aa_locations[0].aa_id, 0);
  ASSERT_EQ(c.drug_db()->at(3)->resistant_aa_locations[0].is_copy_number, true);

  ASSERT_EQ(c.pf_gene_info().chromosome_infos[chromosome_id].gene_infos[gene_id].name, "Pfplasmepsin");
  ASSERT_EQ(c.pf_gene_info().chromosome_infos[chromosome_id].gene_infos[gene_id].max_copies, 2);
}

TEST(ConfigTest, ReadAAPostInAASequence) {
  Config c;
  c.read_from_file("input.yml");

  // as
  auto aa_sequence = "||||NY1||KTHFI,x||||||FNMYRIPRPC|1";
  auto aa_pos1 = c.drug_db()->at(0)->resistant_aa_locations[0].aa_index_in_aa_string;
  ASSERT_EQ(aa_sequence[aa_pos1], 'F');

  // pip
  auto aa_pos2 = c.drug_db()->at(3)->resistant_aa_locations[0].aa_index_in_aa_string;
  ASSERT_EQ(aa_sequence[aa_pos2], '1');

  // lm
  auto aa_pos3 = c.drug_db()->at(1)->resistant_aa_locations[2].aa_index_in_aa_string;
  ASSERT_EQ(aa_sequence[aa_pos3], '1');

  auto aa_pos4 = c.drug_db()->at(1)->resistant_aa_locations[3].aa_index_in_aa_string;
  ASSERT_EQ(aa_sequence[aa_pos4], 'K');

  // kaf
  //  auto aa_pos5 = c.drug_db()->at(7)->resistant_aa_location[0].aa_index_in_aa_string;
  //  ASSERT_EQ(aa_sequence[aa_pos5], 'x');
}