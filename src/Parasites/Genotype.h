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
typedef std::string GeneStr;
typedef std::vector<GeneStr> ChromosomeStr;
typedef std::array<ChromosomeStr, 14> AaStructure;

class Genotype {
  DISALLOW_COPY_AND_ASSIGN(Genotype)

public:
  int genotype_id { -1 };
  AaStructure aa_structure {};
  std::string aa_sequence;
  double daily_fitness_multiple_infection { 1 };
  std::vector<double> EC50_power_n {};

public:
  explicit Genotype(const std::string& aa_sequence);

  virtual ~Genotype();

  double get_EC50_power_n(DrugType* dt) const;

  bool resist_to(DrugType* dt);

  bool resist_to(Therapy* therapy);

  Genotype* combine_mutation_to(const int& locus, const int& value);

  int select_mutation_allele(const int& mutation_locus);

  std::string get_aa_sequence() const;

  bool is_valid(const PfGeneInfo& gene_info);

  void calculate_daily_fitness(const PfGeneInfo& gene_info);

  friend std::ostream& operator<<(std::ostream& os, const Genotype& e);
  void calculate_EC50_power_n(const PfGeneInfo& info, DrugDatabase* pDatabase);
};

#endif /* Genotype_H */
