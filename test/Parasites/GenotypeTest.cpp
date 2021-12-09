//
// Created by nguyentd on 12/7/2021.
//

#include <gtest/gtest.h>
#include "Parasites/Genotype.h"

class GenotypeTest : public ::testing::Test {
protected:
  void SetUp() override {

  }

  // void TearDown() override {}

  Genotype g;
};

TEST_F(GenotypeTest, EmptyGetGeneString) {
  EXPECT_EQ(g.get_aa_sequence(), "|||||||||||||");
}

TEST_F(GenotypeTest, GetGeneStringShouldProduceRighthContent) {
  for (int zz = 0; zz < 14; ++zz) {
    g.aa_structure[zz] = std::string_view("ATCG");
  }

  EXPECT_EQ(g.get_aa_sequence(), "ATCG|ATCG|ATCG|ATCG|ATCG|ATCG|ATCG|ATCG|ATCG|ATCG|ATCG|ATCG|ATCG|ATCG");
}

