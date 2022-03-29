//
// Created by nguyentd on 3/29/2022.
//
#include "Core/Random.h"
#include "Population/Person.h"
#include "gtest/gtest.h"

class RouletteTest : public ::testing::Test {
protected:
  void SetUp() override {
    r.initialize(0);

    for (int i = 0; i < n_person; ++i) {
      auto* p = new Person();
      p->set_last_therapy_id(i);
      all_person.push_back(p);
      distribution.push_back(r.random_uniform());
    }
  }

  void TearDown() override {
    for (auto* p : all_person) {
      delete p;
    }

    all_person.clear();
    distribution.clear();
  }

  Random r;
  int n_sample { 10 };
  int n_person { 100 };

  std::vector<Person*> all_person;
  std::vector<double> distribution;
};

TEST_F(RouletteTest, Sampling_with_sum__distribution_0) {
  auto results = r.roulette_sampling<Person>(n_sample, distribution, all_person, false, 0);

  EXPECT_EQ(results.size(), n_sample);
  EXPECT_EQ(results[0], nullptr);
}
