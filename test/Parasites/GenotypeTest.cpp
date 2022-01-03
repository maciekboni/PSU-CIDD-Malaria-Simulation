//
// Created by nguyentd on 12/7/2021.
//

#include <gtest/gtest.h>

#include "Parasites/Genotype.h"

class GenotypeTest : public ::testing::Test {
protected:
  void SetUp() override {}

  // void TearDown() override {}
};

TEST_F(GenotypeTest, InitializeWithAASeuquence) {
  Genotype g("||||NY1||KTHFI||||||FNMYRIPRPC|1");

  EXPECT_EQ(g.aa_structure[0], "");
  EXPECT_EQ(g.aa_structure[4], "NY1");
  EXPECT_EQ(g.aa_structure[6], "KTHFI");
  EXPECT_EQ(g.aa_structure[12], "FNMYRIPRPC");
  EXPECT_EQ(g.aa_structure[13], "1");

  EXPECT_EQ(g.get_aa_sequence(), "||||NY1||KTHFI||||||FNMYRIPRPC|1");
}
