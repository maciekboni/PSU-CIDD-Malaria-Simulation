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

GenotypeDatabase::GenotypeDatabase() : auto_id(0) {}

GenotypeDatabase::~GenotypeDatabase() {
  for (auto &i : *this) {
    delete i.second;
  }
  clear();
}

void GenotypeDatabase::add(Genotype *genotype) {
  if (this->find(genotype->genotype_id()) != this->end()) {
    delete (*this)[genotype->genotype_id()];
  }
  (*this)[genotype->genotype_id()] = genotype;
}

Genotype *GenotypeDatabase::get_genotype_from_alleles_structure(const IntVector &alleles) {
  auto id = 0;
  for (auto i = 0; i < alleles.size(); i++) {
    id += weight_[i] * alleles[i];
  }
  return this->at(id);
}

unsigned int GenotypeDatabase::get_id(const std::string &aa_sequence) {
  if (aa_sequence_id_map.find(aa_sequence) == aa_sequence_id_map.end()) {
    // not yet exist then initialize new genotype
    auto new_id = auto_id;
    aa_sequence_id_map[aa_sequence] = new_id;

    auto new_genotype = new Genotype(aa_sequence);
    new_genotype->set_genotype_id(auto_id);

    add(new_genotype);

    auto_id++;
    return new_id;
  } else {
    return aa_sequence_id_map[aa_sequence];
  }
}
