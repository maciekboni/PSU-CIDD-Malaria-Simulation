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

// TODO: add tests for 3 or more genes on same chromosome

TEST(GenotypeFreeRecombinationTest, NoWithinChromosomeFreeRecombine) {
  Config c;
  c.read_from_file("input.yml");
  c.within_chromosome_recombination_rate() = 0;

  auto genotype_A = c.genotype_db.get_genotype("||||NY1||KTHFI,x||||||FNMYRIPRPC|1", &c);
  auto genotype_B = c.genotype_db.get_genotype("||||YF2||TTHFI,X||||||FNMYRIPRPY|2", &c);

  auto genotype_C_str = "||||NY1||TTHFI,X||||||FNMYRIPRPC|2";

  MockRandom random;
  EXPECT_CALL(random, random_uniform())
      .WillOnce(Return(0))
      .WillOnce(Return(0))  // check within chromosome recombination rate
      .WillOnce(Return(0.6))
      .WillOnce(Return(0.3))
      .WillOnce(Return(0.6))
      .WillRepeatedly(Return(0));
  //  EXPECT_CALL(random, random_uniform(_)).WillOnce(Return(0)).WillRepeatedly(Return(0));

  auto genotype_C = genotype_A->free_recombine_with(&c, &random, genotype_B);
  EXPECT_EQ(genotype_C->aa_sequence, genotype_C_str);
}

TEST(GenotypeFreeRecombinationTest, WithinChromosomeFreeRecombineTopBottom) {
  Config c;
  c.read_from_file("input.yml");

  auto genotype_A = c.genotype_db.get_genotype("||||NY1||KTHFI,x||||||FNMYRIPRPC|1", &c);
  auto genotype_B = c.genotype_db.get_genotype("||||YF2||TTHFI,X||||||FNMYRIPRPY|2", &c);

  auto genotype_C_str = "||||NY1||KTHFI,X||||||FNMYRIPRPC|2";

  MockRandom random;
  EXPECT_CALL(random, random_uniform(_)).WillOnce(Return(0)).WillRepeatedly(Return(0));

  EXPECT_CALL(random, random_uniform())
      .WillOnce(Return(0))
      .WillOnce(Return(0.05))  // check within chromosome recombination rate
      .WillOnce(Return(0.3))   // top-bottom
      .WillOnce(Return(0.3))
      .WillOnce(Return(0.6))
      .WillRepeatedly(Return(0));

  auto genotype_C = genotype_A->free_recombine_with(&c, &random, genotype_B);
  EXPECT_EQ(genotype_C->aa_sequence, genotype_C_str);
}

TEST(GenotypeFreeRecombinationTest, WithinChromosomeFreeRecombineBottomTop) {
  Config c;
  c.read_from_file("input.yml");

  auto genotype_A = c.genotype_db.get_genotype("||||NY1||KTHFI,x||||||FNMYRIPRPC|1", &c);
  auto genotype_B = c.genotype_db.get_genotype("||||YF2||TTHFI,X||||||FNMYRIPRPY|2", &c);

  auto genotype_C_str = "||||NY1||TTHFI,x||||||FNMYRIPRPC|2";

  MockRandom random;
  EXPECT_CALL(random, random_uniform(_)).WillOnce(Return(0)).WillRepeatedly(Return(0));

  EXPECT_CALL(random, random_uniform())
      .WillOnce(Return(0))
      .WillOnce(Return(0.05))  // check within chromosome recombination rate
      .WillOnce(Return(0.6))   // bottom-top
      .WillOnce(Return(0.3))
      .WillOnce(Return(0.6))
      .WillRepeatedly(Return(0));

  auto genotype_C = genotype_A->free_recombine_with(&c, &random, genotype_B);
  EXPECT_EQ(genotype_C->aa_sequence, genotype_C_str);

}