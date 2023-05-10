//
// Created by nguyentd on 3/29/2022.
//
#include <fmt/format.h>

#include <chrono>
#include <numeric>

#include "Core/Random.h"
#include "Population/Person.h"
#include "gtest/gtest.h"

class RouletteTest : public ::testing::Test {
protected:
    void SetUp() override {
        r.initialize(0);

        for (int i = 0; i < n_person; ++i) {
            auto *p = new Person();
            p->set_last_therapy_id(i);
            all_person.push_back(p);
            distribution.push_back(r.random_uniform());
        }
    }

    void TearDown() override {
        for (auto *p: all_person) {
            delete p;
        }

        all_person.clear();
        distribution.clear();
    }

    Random r;
    int n_sample{10};
    int n_person{1000};

    std::vector<Person *> all_person;
    std::vector<double> distribution;
};

TEST_F(RouletteTest, Sampling_with_sum_distribution_0) {
    auto results = r.roulette_sampling<Person>(n_sample, distribution, all_person, false, 0);

    EXPECT_EQ(results.size(), n_sample);
    EXPECT_EQ(results[0], nullptr);
}

TEST_F(RouletteTest, Sampling_with_no_sum_distribution) {
    std::vector<double> foi_distribution(n_person, 1.0);

    // even id person will have no selection
    for (int i = 0; i < n_person; i += 2) {
        foi_distribution[i] = 0;
    }
    for (int n = 0; n < 10; ++n) {
        auto results = r.roulette_sampling<Person>(n_sample, foi_distribution, all_person, false);

        EXPECT_EQ(results.size(), n_sample);

        for (int i = 0; i < n_sample; ++i) {
            std::cout << results[i]->last_therapy_id() << "\t";
            EXPECT_EQ(results[i]->last_therapy_id() % 2, 1)
                                << fmt::format("failed with p_id: {}", results[i]->last_therapy_id());
        }
        std::cout << std::endl;
    }
}

TEST_F(RouletteTest, Sampling_with_one_in_all) {
    std::vector<double> foi_distribution(n_person, 0.0);

    foi_distribution[n_person - 1] = 1;

    for (int n = 0; n < 10; ++n) {
        auto results = r.roulette_sampling<Person>(n_sample, foi_distribution, all_person, false, 1);

        EXPECT_EQ(results.size(), n_sample);

        for (int i = 0; i < n_sample; ++i) {
            std::cout << results[i]->last_therapy_id() << "\t";
            EXPECT_EQ(results[i]->last_therapy_id(), n_person - 1)
                                << fmt::format("failed with p_id: {}", results[i]->last_therapy_id());
        }
        std::cout << std::endl;
    }
}

TEST_F(RouletteTest, Sampling_with_2_in_all) {
    std::vector<double> foi_distribution(n_person, 0.0);

    foi_distribution[n_person - 1] = 0.2;
    foi_distribution[0] = 1.8;
    auto sum = foi_distribution[n_person - 1] + foi_distribution[0];
    auto count{0}, n_repeat{10};

    for (int n = 0; n < n_repeat; ++n) {
        auto results = r.roulette_sampling<Person>(n_sample, foi_distribution, all_person, true);

        EXPECT_EQ(results.size(), n_sample);

        for (int i = 0; i < n_sample; ++i) {
            std::cout << results[i]->last_therapy_id() << "\t";
            EXPECT_TRUE(results[i]->last_therapy_id() == (0) || results[i]->last_therapy_id() == (n_person - 1))
                                << fmt::format("failed with p_id: {}", results[i]->last_therapy_id());
            if (results[i]->last_therapy_id() == (n_person - 1)) {
                count++;
            }
        }
        std::cout << std::endl;
    }

    std::cout << fmt::format("Expected - Actual freq: {} - {}", foi_distribution[n_person - 1] / sum,
                             count / (double) (n_repeat * n_sample))
              << std::endl;
    //  EXPECT_NEAR(foi_distribution[n_person - 1] / sum, count / (double)(n_repeat * n_sample), 0.02);
}

TEST_F(RouletteTest, Sampling_with_4_in_all) {
    std::vector<double> foi_distribution(n_person, 0.0);

    foi_distribution[0] = 1;
    foi_distribution[100] = 0.2;
    foi_distribution[200] = 0.5;
    foi_distribution[300] = 0.8;

    auto sum = foi_distribution[0] + foi_distribution[100] + foi_distribution[200] + foi_distribution[300];
    auto n_repeat{10000};

    std::map<int, int> count = {
            {0,   0},
            {100, 0},
            {200, 0},
            {300, 0},
    };

    for (int n = 0; n < n_repeat; ++n) {
        auto results = r.roulette_sampling<Person>(n_sample, foi_distribution, all_person, true, 2.5);

        EXPECT_EQ(results.size(), n_sample);

        for (int i = 0; i < n_sample; ++i) {
            EXPECT_TRUE(results[i]->last_therapy_id() == 0 || results[i]->last_therapy_id() == 100
                        || results[i]->last_therapy_id() == 200 || results[i]->last_therapy_id() == 300)
                                << fmt::format("failed with p_id: {}", results[i]->last_therapy_id());
            count[results[i]->last_therapy_id()]++;
        }
    }

    std::cout << fmt::format("{} trials, {} samples per trial", n_repeat, n_sample) << std::endl;
    for (const auto &[key, value]: count) {
        std::cout << fmt::format("Key: {} \tExpected - Actual freq: {} - {}", key, foi_distribution[key] / sum,
                                 value / (double) (n_repeat * n_sample))
                  << std::endl;
        EXPECT_NEAR(foi_distribution[key] / sum, value / (double) (n_repeat * n_sample), 0.01);
    }
}

TEST_F(RouletteTest, compare_with_multi_normial) {
    const auto size = 1000000;
    std::vector<Person *> population;
    population.reserve(size);
    std::vector<double> foi_distribution;
    foi_distribution.reserve(size);
    for (int i = 0; i < size; ++i) {
        auto *p = new Person();
        p->set_last_therapy_id(i);
        population.push_back(p);
        foi_distribution.push_back(r.random_uniform());
    }

    auto sum = std::accumulate(foi_distribution.begin(), foi_distribution.end(), 0.0);
    auto n_repeat{10000};
    const auto samples = 20000;

//    std::map<int, int> count = {
//            {0,   0},
//            {100, 0},
//            {200, 0},
//            {300, 0},
//    };
    // ==================== roulette sampling ===========================
    auto start = std::chrono::high_resolution_clock::now();
    for (int n = 0; n < n_repeat; ++n) {
        auto results = r.roulette_sampling<Person>(samples, foi_distribution, population, true);

        EXPECT_EQ(results.size(), samples);

//        for (int i = 0; i < n_sample; ++i) {
//            EXPECT_TRUE(results[i]->last_therapy_id() == 0 || results[i]->last_therapy_id() == 100
//                        || results[i]->last_therapy_id() == 200 || results[i]->last_therapy_id() == 300)
//                                << fmt::format("failed with p_id: {}", results[i]->last_therapy_id());
//            count[results[i]->last_therapy_id()]++;
//        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << fmt::format("{} trials, {} samples per trial", n_repeat, samples) << std::endl;
    std::cout << fmt::format("Roulette Sampling times: {}ms", duration.count()) << std::endl;

    // ==================== multinomial sampling ===========================
    start = std::chrono::high_resolution_clock::now();
    foi_distribution.resize(100);
    for (int n = 0; n < n_repeat; ++n) {
        auto results = r.multinomial_sampling<Person>(samples, foi_distribution, population, true, sum);

        EXPECT_EQ(results.size(), samples);

//        for (int i = 0; i < n_sample; ++i) {
//            EXPECT_TRUE(results[i]->last_therapy_id() == 0 || results[i]->last_therapy_id() == 100
//                        || results[i]->last_therapy_id() == 200 || results[i]->last_therapy_id() == 300)
//                                << fmt::format("failed with p_id: {}", results[i]->last_therapy_id());
//            count[results[i]->last_therapy_id()]++;
//        }
        for (int jj = 0; jj < samples; ++jj) {
            r.random_uniform();
        }
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    for (auto &p: population) {
        delete p;
    }

    std::cout << fmt::format("Multinomial Sampling times: {}ms", duration.count()) << std::endl;

}
