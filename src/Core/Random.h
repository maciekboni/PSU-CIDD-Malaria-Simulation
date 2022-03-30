/*
 * File:   Random.h
 * Author: nguyentran
 *
 * Created on May 27, 2013, 10:46 AM
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <gsl/gsl_rng.h>

#include <algorithm>

#include "PropertyMacro.h"
#include "Strategies/AdaptiveCyclingStrategy.h"

class Model;

class Random {
  DISALLOW_COPY_AND_ASSIGN(Random)

  DISALLOW_MOVE(Random)

  VIRTUAL_PROPERTY(unsigned long, seed)

public:
  gsl_rng *G_RNG;

  explicit Random(gsl_rng *g_rng = nullptr);

  virtual ~Random();

  void initialize(const unsigned long &seed = 0);

  void release() const;

  virtual int random_poisson(const double &poisson_mean);

  virtual double random_uniform_double(const double &from, const double &to);

  /*
   * This function will return a random number in [0,1)
   */
  virtual double random_uniform();

  virtual unsigned long random_uniform(unsigned long range);

  virtual unsigned long random_uniform_int(const unsigned long &from, const unsigned long &to);

  virtual double random_normal(const double &mean, const double &sd);

  virtual double random_normal_truncated(const double &mean, const double &sd);

  virtual int random_normal(const int &mean, const int &sd);

  virtual int random_normal_truncated(const int &mean, const int &sd);

  virtual double random_beta(const double &alpha, const double &beta);

  virtual double random_gamma(const double &a, const double &b);

  virtual double cdf_gamma_distribution(const double &x, const double &alpha, const double &beta);

  virtual double cdf_gamma_distribution_inverse(const double &p, const double &alpha, const double &beta);

  virtual double random_flat(const double &from, const double &to);

  virtual void random_multinomial(const size_t &K, const unsigned &N, double p[], unsigned n[]);

  virtual void random_shuffle(void *base, size_t base_length, size_t size_of_type);

  virtual double cdf_standard_normal_distribution(const double &p);

  virtual int random_binomial(const double &p, const unsigned int &n);

  void shuffle(void *base, const size_t &n, const size_t &size);

  template <class T>
  [[nodiscard]] std::vector<T *> multinomial_sampling(int size, std::vector<double> &distribution,
                                                      std::vector<T *> &all_objects, bool is_shuffled,
                                                      double sum_distribution = -1);

  template <class T>
  [[nodiscard]] std::vector<T *> roulette_sampling(int number_of_samples, std::vector<double> &distribution,
                                                   std::vector<T *> &all_objects, bool is_shuffled,
                                                   double sum_distribution = -1);
};

template <class T>
std::vector<T *> Random::multinomial_sampling(int size, std::vector<double> &distribution,
                                              std::vector<T *> &all_objects, bool is_shuffled,
                                              double sum_distribution) {
  std::vector<T *> samples(size, nullptr);
  if (sum_distribution == 0) {
    return samples;
  } else if (sum_distribution < 0) {
    auto found = std::find_if(distribution.begin(), distribution.end(), [](double d) { return d > 0; });

    if (found == distribution.end()) {
      return samples;
    }
  }

  std::vector<unsigned int> hit_per_object(distribution.size());
  random_multinomial(distribution.size(), size, &distribution[0], &hit_per_object[0]);

  auto index = 0;
  for (auto i = 0; i < hit_per_object.size(); i++) {
    for (int j = 0; j < hit_per_object[i]; ++j) {
      samples[index] = all_objects[i];
      index++;
    }
  }
  if (is_shuffled) {
    random_shuffle(&samples[0], samples.size(), sizeof(T *));
  }
  return samples;
}

template <class T>
std::vector<T *> Random::roulette_sampling(int number_of_samples, std::vector<double> &distribution,
                                           std::vector<T *> &all_objects, bool is_shuffled, double sum_distribution) {
  std::vector<T *> samples(number_of_samples, nullptr);
  double sum { sum_distribution };
  if (sum_distribution == 0) {
    return samples;
  } else if (sum_distribution < 0) {
    sum = 0;
    for (auto d : distribution) {
      sum += d;
    }
  }

  std::vector<double> uniform_sampling(number_of_samples, 0.0);
  for (auto &index : uniform_sampling) {
    index = this->random_uniform() * sum;
  }

  std::sort(uniform_sampling.begin(), uniform_sampling.end());

  double sum_weight = 0;
  int uniform_sampling_index = 0;

  for (auto pi = 0; pi < distribution.size(); pi++) {
    auto weight = distribution[pi];
    sum_weight += weight;
    while (uniform_sampling_index < number_of_samples && uniform_sampling[uniform_sampling_index] < sum_weight) {
      samples[uniform_sampling_index] = all_objects[pi];
      uniform_sampling_index++;
    }
    if (uniform_sampling_index == number_of_samples) {
      break;
    }
  }
  if (is_shuffled) {
    random_shuffle(&samples[0], samples.size(), sizeof(T *));
  }
  return samples;
}
#endif /* RANDOM_H */
