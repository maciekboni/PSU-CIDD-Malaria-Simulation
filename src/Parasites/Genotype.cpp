/* 
 * File:   Genotype.cpp
 * Author: Merlin
 * 
 * Created on March 17, 2014, 2:33 PM
 */

#include "Genotype.h"
#include "Therapies/DrugDatabase.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Therapies/SCTherapy.h"

Genotype::Genotype(const int &id, const GenotypeInfo &genotype_info, const IntVector &weight) : genotype_id_(id) {

}

Genotype::~Genotype() = default;

bool Genotype::resist_to(DrugType* dt) {
  // TODO: rework on this
  return false;
}

bool Genotype::resist_to(Therapy* therapy) {
  // TODO: rework on this
  return false;
}

Genotype* Genotype::combine_mutation_to(const int &locus, const int &value) {
  // TODO: rework on this
  return this;
}

double Genotype::get_EC50_power_n(DrugType* dt) const {

  return get_EC50(dt->id());
}

double Genotype::get_EC50(const int &drug_id) const {

  return Model::CONFIG->EC50_power_n_table()[genotype_id_][drug_id];
}

int Genotype::select_mutation_allele(const int &mutation_locus) {
  // TODO: rework on this
  return 0;
}

std::ostream &operator<<(std::ostream &os, const Genotype &e) {
  os << e.genotype_id_ << "\t";
  os << e.get_gene_string();
//    os << "\t" << e.number_of_resistance_position_;
  return os;
}

std::string Genotype::get_gene_string() const {
  std::stringstream ss;


//  for (auto i = 0; i < aa_structure.size(); i++) {
//    const auto v = aa_structure[i];
//    ss << Model::CONFIG->genotype_info().loci_vector[i].alleles[v];
//  }

  return ss.str();
}
