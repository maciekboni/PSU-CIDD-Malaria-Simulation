/*
 * File:   Genotype.h
 * Author: Merlin
 *
 * Created on March 17, 2014, 2:33 PM
 */

#ifndef Genotype_H
#define Genotype_H

#include <array>

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"

class DrugDatabase;

class DrugType;

class Therapy;

class Config;

class Random;

typedef std::string GeneStr;
typedef std::vector<GeneStr> ChromosomalGenotypeStr;
typedef std::array<ChromosomalGenotypeStr, 14> PfGenotypeStr;

class Genotype {
  DISALLOW_COPY_AND_ASSIGN(Genotype)

public:
  int genotype_id { -1 };
  PfGenotypeStr pf_genotype_str {};
  std::string aa_sequence;
  double daily_fitness_multiple_infection { 1 };
  std::vector<double> EC50_power_n {};

public:
  explicit Genotype(const std::string& aa_sequence);

  virtual ~Genotype();

  double get_EC50_power_n(DrugType* dt) const;

  bool resist_to(DrugType* dt);

  Genotype* combine_mutation_to(const int& locus, const int& value);

  std::string get_aa_sequence() const;

  bool is_valid(const PfGeneInfo& gene_info);

  void calculate_daily_fitness(const PfGeneInfo& gene_info);

  void calculate_EC50_power_n(const PfGeneInfo& info, DrugDatabase* pDatabase);

  Genotype* perform_mutation_by_drug(Config* pConfig, Random* pRandom, DrugType* pDrugType, double mutation_probability_by_locus) const;

  friend std::ostream& operator<<(std::ostream& os, const Genotype& e);

  void override_EC50_power_n(const std::vector<OverrideEC50Pattern>& override_patterns, DrugDatabase* drug_db);

  bool match_pattern(const std::string& pattern);

  Genotype* free_recombine_with(Config* config, Random* pRandom, Genotype* other);

  static Genotype* free_recombine(Config* config, Random* pRandom, Genotype* f, Genotype* m);

  static std::string Convert_PfGenotypeStr_To_String(const PfGenotypeStr& pfGenotypeStr);
};

#endif /* Genotype_H */
