/*
 * File:   IntParasiteDatabase.cpp
 * Author: Merlin
 *
 * Created on March 18, 2014, 3:06 PM
 */

#include "GenotypeDatabase.h"

#include "Core/Config/Config.h"
#include "Genotype.h"
#include "Helpers/NumberHelpers.h"
#include "Model.h"

GenotypeDatabase::GenotypeDatabase() : auto_id(0) {}

GenotypeDatabase::~GenotypeDatabase() {
  for (auto &i : *this) {
    delete i.second;
  }
  clear();
}

void GenotypeDatabase::add(Genotype *genotype) {
  if (this->find(genotype->genotype_id) != this->end()) {
    delete (*this)[genotype->genotype_id];
  }
  (*this)[genotype->genotype_id] = genotype;
}

Genotype *GenotypeDatabase::get_genotype_from_alleles_structure(const IntVector &alleles) {
  auto id = 0;
  for (auto i = 0; i < alleles.size(); i++) {
    id += weight_[i] * alleles[i];
  }
  return this->at(id);
}

unsigned int GenotypeDatabase::get_id(const std::string &aa_sequence, Config *config) {
  if (aa_sequence_id_map.find(aa_sequence) == aa_sequence_id_map.end()) {
    // not yet exist then initialize new genotype
    auto new_id = auto_id;
    aa_sequence_id_map[aa_sequence] = new_id;

    auto new_genotype = new Genotype(aa_sequence);
    new_genotype->genotype_id = new_id;

    // check if aa_sequence is valid
    if (!new_genotype->is_valid(config->pf_gene_info())) {
      LOG(FATAL) << "Invalid genotype: " << aa_sequence;
    }

    // calculate cost of resistance
    new_genotype->calculate_daily_fitness(config->pf_gene_info());

    // calculate ec50
    new_genotype->calculate_EC50_power_n(config->pf_gene_info(), config->drug_db());
    add(new_genotype);

    auto_id++;
    return new_id;
  } else {
    return aa_sequence_id_map[aa_sequence];
  }
}
