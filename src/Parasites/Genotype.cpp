/*
 * File:   Genotype.cpp
 * Author: Merlin
 *
 * Created on March 17, 2014, 2:33 PM
 */
#include "Genotype.h"

#include <algorithm>

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Helpers/NumberHelpers.h"
#include "Model.h"
#include "Therapies/DrugDatabase.h"
#include "Therapies/SCTherapy.h"

Genotype::Genotype(const std::string &in_aa_sequence) : aa_sequence { in_aa_sequence } {
  // create aa structure
  std::string chromosome_str;
  std::istringstream tokenStream(in_aa_sequence);
  auto ii = 0;
  while (std::getline(tokenStream, chromosome_str, '|')) {
    std::string gene_str;
    std::istringstream chromosomeTokenStream(chromosome_str);
    auto jj = 0;
    while (std::getline(chromosomeTokenStream, gene_str, ',')) {
      pf_genotype_str[ii].push_back(gene_str);
      jj++;
    }
    ii++;
  }
}

Genotype::~Genotype() = default;

bool Genotype::resist_to(DrugType *dt) {
  return EC50_power_n[dt->id()] > pow(dt->base_EC50, dt->n());
}

Genotype *Genotype::combine_mutation_to(const int &locus, const int &value) {
  // TODO: remove this
  return this;
}

double Genotype::get_EC50_power_n(DrugType *dt) const {
  return EC50_power_n[dt->id()];
}

std::ostream &operator<<(std::ostream &os, const Genotype &e) {
  os << e.genotype_id << "\t";
  os << e.get_aa_sequence();
  return os;
}

std::string Genotype::get_aa_sequence() const {
  return aa_sequence;
}
bool Genotype::is_valid(const PfGeneInfo &gene_info) {
  for (int chromosome_i = 0; chromosome_i < 14; ++chromosome_i) {
    auto chromosome_info = gene_info.chromosome_infos[chromosome_i];
    if (chromosome_info.gene_infos.size() != pf_genotype_str[chromosome_i].size()) return false;

    for (int gene_i = 0; gene_i < pf_genotype_str[chromosome_i].size(); ++gene_i) {
      auto gene_info = chromosome_info.gene_infos[gene_i];
      auto max_aa_pos = gene_info.max_copies > 1 ? pf_genotype_str[chromosome_i][gene_i].size() - 1
                                                 : pf_genotype_str[chromosome_i][gene_i].size();

      // check same size with aa postions info
      if (gene_info.aa_position_infos.size() != max_aa_pos) {
        std::cout << pf_genotype_str[chromosome_i][gene_i] << std::endl;
        std::cout << gene_info.aa_position_infos.size() << std::endl;
        return false;
      }

      for (int aa_i = 0; aa_i < max_aa_pos; ++aa_i) {
        auto aa_pos_info = gene_info.aa_position_infos[aa_i];
        auto element = pf_genotype_str[chromosome_i][gene_i][aa_i];

        if (std::find(aa_pos_info.amino_acids.begin(), aa_pos_info.amino_acids.end(), element)
            == aa_pos_info.amino_acids.end())
          return false;
      }

      // check number copy valid or not
      if (gene_info.max_copies > 1) {
        auto copy_number = NumberHelpers::char_to_single_digit_number(pf_genotype_str[chromosome_i][gene_i].back());
        if (copy_number > gene_info.max_copies) {
          return false;
        }
      }
    }
  }
  return true;
}
void Genotype::calculate_daily_fitness(const PfGeneInfo &gene_info) {
  daily_fitness_multiple_infection = 1.0;

  for (int chromosome_i = 0; chromosome_i < pf_genotype_str.size(); ++chromosome_i) {
    auto chromosome_info = gene_info.chromosome_infos[chromosome_i];

    for (int gene_i = 0; gene_i < pf_genotype_str[chromosome_i].size(); ++gene_i) {
      auto res_gene_info = chromosome_info.gene_infos[gene_i];
      auto max_aa_pos = res_gene_info.max_copies > 1 ? pf_genotype_str[chromosome_i][gene_i].size() - 1
                                                     : pf_genotype_str[chromosome_i][gene_i].size();
      for (int aa_i = 0; aa_i < max_aa_pos; ++aa_i) {
        // calculate cost of resistance
        auto aa_pos_info = res_gene_info.aa_position_infos[aa_i];
        auto element = pf_genotype_str[chromosome_i][gene_i][aa_i];

        auto it = std::find(aa_pos_info.amino_acids.begin(), aa_pos_info.amino_acids.end(), element);
        auto element_id = it - aa_pos_info.amino_acids.begin();

        auto cr = aa_pos_info.daily_crs[element_id];

        daily_fitness_multiple_infection *= (1 - cr);
      }

      // calculate for number copy variation
      if (res_gene_info.max_copies > 1) {
        auto copy_number = (int)pf_genotype_str[chromosome_i][gene_i].back() - 48;
        if (copy_number > 1) {
          daily_fitness_multiple_infection *= 1 - res_gene_info.cnv_daily_crs[copy_number - 1];
        }
      }
    }
  }
}

void Genotype::calculate_EC50_power_n(const PfGeneInfo &gene_info, DrugDatabase *drug_db) {
  EC50_power_n.resize(drug_db->size());
  for (const auto &[drug_id, dt] : *drug_db) {
    EC50_power_n[drug_id] = dt->base_EC50;
  }

  for (int chromosome_i = 0; chromosome_i < pf_genotype_str.size(); ++chromosome_i) {
    auto chromosome_info = gene_info.chromosome_infos[chromosome_i];

    for (int gene_i = 0; gene_i < pf_genotype_str[chromosome_i].size(); ++gene_i) {
      auto res_gene_info = chromosome_info.gene_infos[gene_i];
      auto max_aa_pos = res_gene_info.max_copies > 1 ? pf_genotype_str[chromosome_i][gene_i].size() - 1
                                                     : pf_genotype_str[chromosome_i][gene_i].size();
      std::vector<int> number_of_effective_mutations_in_same_genes(drug_db->size(), 0);

      for (int aa_i = 0; aa_i < max_aa_pos; ++aa_i) {
        // calculate cost of resistance
        auto aa_pos_info = res_gene_info.aa_position_infos[aa_i];
        auto element = pf_genotype_str[chromosome_i][gene_i][aa_i];
        auto it = std::find(aa_pos_info.amino_acids.begin(), aa_pos_info.amino_acids.end(), element);
        if (it == aa_pos_info.amino_acids.end()) {
          LOG(FATAL) << "Incorrect AA in aa sequence";
        }
        auto element_id = it - aa_pos_info.amino_acids.begin();

        for (const auto &[drug_id, dt] : *drug_db) {
          if (aa_pos_info.multiplicative_effect_on_EC50.find(drug_id)
              != aa_pos_info.multiplicative_effect_on_EC50.end()) {
            if (aa_pos_info.multiplicative_effect_on_EC50[drug_id][element_id] > 1) {
            }
            auto multiplicative_effect_factor = aa_pos_info.multiplicative_effect_on_EC50[drug_id][element_id];

            if (multiplicative_effect_factor > 1) {
              // encounter resistant aa
              number_of_effective_mutations_in_same_genes[drug_id] += 1;
              if (number_of_effective_mutations_in_same_genes[drug_id] > 1
                  && res_gene_info.multiplicative_effect_on_EC50_for_2_or_more_mutations.find(drug_id)
                         != res_gene_info.multiplicative_effect_on_EC50_for_2_or_more_mutations.end()) {
                // if multiplicative effect can apply to this drug
                multiplicative_effect_factor =
                    res_gene_info.multiplicative_effect_on_EC50_for_2_or_more_mutations[drug_id];
              }
            }
            EC50_power_n[drug_id] *= multiplicative_effect_factor;
          }
        }
      }

      // calculate for number copy variation
      if (res_gene_info.max_copies > 1) {
        auto copy_number = (int)pf_genotype_str[chromosome_i][gene_i].back() - 48;
        if (copy_number > 1) {
          for (const auto &[drug_id, dt] : *drug_db) {
            if (res_gene_info.cnv_multiplicative_effect_on_EC50.find(drug_id)
                != res_gene_info.cnv_multiplicative_effect_on_EC50.end()) {
              EC50_power_n[drug_id] *= res_gene_info.cnv_multiplicative_effect_on_EC50[drug_id][copy_number - 1];
            }
          }
        }
      }
    }
  }

  // power n
  for (const auto &[drug_id, dt] : *drug_db) {
    EC50_power_n[drug_id] = pow(EC50_power_n[drug_id], dt->n());
  }
}

Genotype *Genotype::perform_mutation_by_drug(Config *pConfig, Random *pRandom, DrugType *pDrugType, double mutation_probability) const {
  std::string new_aa_sequence { aa_sequence };
  for(int aa_pos_id = 0; aa_pos_id < pDrugType->resistant_aa_locations.size(); aa_pos_id++) {
    // get aa position info (aa index in aa string, is copy number)
    auto aa_pos = pDrugType->resistant_aa_locations[aa_pos_id];
    if(pConfig->mutation_mask()[aa_pos.aa_index_in_aa_string] == '1'){
//        LOG(INFO) << "Drug: " << pDrugType->name() << " genotype: " << aa_sequence << " mutation pos: " << aa_pos.aa_index_in_aa_string << " is 1";
        const auto p = Model::RANDOM->random_flat(0.0, 1.0);
        if (p < mutation_probability){
//            LOG(INFO) << "\tDrug: " << pDrugType->name() << " genotype: " << aa_sequence << " mutation happened at aa_pos_id: " << aa_pos_id << " aa_pos: " << aa_pos.aa_index_in_aa_string;
            if (aa_pos.is_copy_number) {
                // increase or decrease by 1 step
                auto old_copy_number = NumberHelpers::char_to_single_digit_number(aa_sequence[aa_pos.aa_index_in_aa_string]);
                if (old_copy_number == 1) {
                    new_aa_sequence[aa_pos.aa_index_in_aa_string] = NumberHelpers::single_digit_number_to_char(old_copy_number + 1);
                } else if (old_copy_number
                           == pConfig->pf_genotype_info()
                                   .chromosome_infos[aa_pos.chromosome_id]
                                   .gene_infos[aa_pos.gene_id]
                                   .max_copies) {
                    new_aa_sequence[aa_pos.aa_index_in_aa_string] = NumberHelpers::single_digit_number_to_char(old_copy_number - 1);
                } else {
                    auto new_copy_number = pRandom->random_uniform() < 0.5 ? old_copy_number - 1 : old_copy_number + 1;
                    new_aa_sequence[aa_pos.aa_index_in_aa_string] = NumberHelpers::single_digit_number_to_char(new_copy_number);
                }

            } else {
                auto &aa_list = pConfig->pf_genotype_info()
                        .chromosome_infos[aa_pos.chromosome_id]
                        .gene_infos[aa_pos.gene_id]
                        .aa_position_infos[aa_pos.aa_id]
                        .amino_acids;
                // draw random aa id
                auto new_aa_id = pRandom->random_uniform(aa_list.size() - 1);

                auto old_aa = aa_sequence[aa_pos.aa_index_in_aa_string];
                auto new_aa = aa_list[new_aa_id];
                if (new_aa == old_aa) {
                    new_aa = aa_list[new_aa_id + 1];
                }
                new_aa_sequence[aa_pos.aa_index_in_aa_string] = new_aa;
//                LOG(INFO) << aa_pos_id <<  " MUTATION: " << aa_pos.aa_index_in_aa_string << " " << old_aa << " -> " << new_aa;
            }
        }
    }
  }
//  if(new_aa_sequence != aa_sequence){
//      LOG(INFO) << "AFTER FOR LOOP new aa sequence: " << new_aa_sequence;
//  }
  // get genotype pointer from gene database based on aa sequence
  return pConfig->genotype_db.get_genotype(new_aa_sequence, pConfig);
}

void Genotype::override_EC50_power_n(const std::vector<OverrideEC50Pattern> &override_patterns, DrugDatabase *drug_db) {
  if (EC50_power_n.size() != drug_db->size()) {
    EC50_power_n.resize(drug_db->size());
  }

  for (const auto &pattern : override_patterns) {
    if (match_pattern(pattern.pattern)) {
      // override ec50 power n
      EC50_power_n[pattern.drug_id] = pow(pattern.ec50, drug_db->at(pattern.drug_id)->n());
    }
  }
}

bool Genotype::match_pattern(const std::string &pattern) {
  int id = 0;
  while (id < aa_sequence.length() && (aa_sequence[id] == pattern[id] || pattern[id] == '.')) {
    id++;
  }
  return id >= aa_sequence.length();
}

Genotype *Genotype::free_recombine_with(Config *pConfig, Random *pRandom, Genotype *other) {
  // TODO: this function is not optimized 100%, use with care
  PfGenotypeStr new_pf_genotype_str;
  // for each chromosome
  for (int chromosome_id = 0; chromosome_id < pf_genotype_str.size(); ++chromosome_id) {
    if (pf_genotype_str[chromosome_id].empty()) continue;
    if (pf_genotype_str[chromosome_id].size() == 1) {
      // if single gene
      // draw random
      auto topOrBottom = pRandom->random_uniform();
      // if < 0.5 take from current, otherwise take from other
      auto gene_str = topOrBottom < 0.5 ? pf_genotype_str[chromosome_id][0] : other->pf_genotype_str[chromosome_id][0];
      new_pf_genotype_str[chromosome_id].push_back(gene_str);
    } else {
      // if multiple genes
      // draw random to determine whether
      // within chromosome recombination happens
      auto with_chromosome_recombination = pRandom->random_uniform();
      if (with_chromosome_recombination < pConfig->within_chromosome_recombination_rate()) {
        // if happen draw a random crossover point based on ','
        auto cutting_gene_id = pRandom->random_uniform(pf_genotype_str[chromosome_id].size() - 1) + 1;
        // draw another random to do top-bottom or bottom-top cross over
        auto topOrBottom = pRandom->random_uniform();
        for (auto gene_id = 0; gene_id < cutting_gene_id; ++gene_id) {
          auto gene_str = topOrBottom < 0.5 ? pf_genotype_str[chromosome_id][gene_id]
                                            : other->pf_genotype_str[chromosome_id][gene_id];
          new_pf_genotype_str[chromosome_id].push_back(gene_str);
        }
        for (auto gene_id = cutting_gene_id; gene_id < pf_genotype_str[chromosome_id].size(); ++gene_id) {
          auto gene_str = topOrBottom < 0.5 ? other->pf_genotype_str[chromosome_id][gene_id]
                                            : pf_genotype_str[chromosome_id][gene_id];
          new_pf_genotype_str[chromosome_id].push_back(gene_str);
        }
      } else {
        // if not do the same with single gene
        auto topOrBottom = pRandom->random_uniform();
        for (int gene_id = 0; gene_id < pf_genotype_str[chromosome_id].size(); ++gene_id) {
          auto gene_str = topOrBottom < 0.5 ? pf_genotype_str[chromosome_id][gene_id]
                                            : other->pf_genotype_str[chromosome_id][gene_id];

          new_pf_genotype_str[chromosome_id].push_back(gene_str);
        }
      }
    }
  }

  auto new_aa_sequence = Convert_PfGenotypeStr_To_String(new_pf_genotype_str);
  return pConfig->genotype_db.get_genotype(new_aa_sequence, pConfig);
}

std::string Genotype::Convert_PfGenotypeStr_To_String(const PfGenotypeStr &pfGenotypeStr) {
  std::stringstream ss;

  for (auto &chromosome : pfGenotypeStr) {
    for (auto &gene : chromosome) {
      ss << gene;
      if (gene != chromosome.back()) {
        ss << ",";
      }
    }

    if (&chromosome != &pfGenotypeStr.back()) {
      ss << "|";
    }
  }

  return ss.str();
}
Genotype *Genotype::free_recombine(Config *config, Random *pRandom, Genotype *f, Genotype *m) {
  PfGenotypeStr new_pf_genotype_str;
  // for each chromosome
  for (int chromosome_id = 0; chromosome_id < f->pf_genotype_str.size(); ++chromosome_id) {
    if (f->pf_genotype_str[chromosome_id].empty()) continue;
    if (f->pf_genotype_str[chromosome_id].size() == 1) {
      // if single gene
      // draw random
      auto topOrBottom = pRandom->random_uniform();
      // if < 0.5 take from current, otherwise take from other
      auto gene_str = topOrBottom < 0.5 ? f->pf_genotype_str[chromosome_id][0] : m->pf_genotype_str[chromosome_id][0];
      new_pf_genotype_str[chromosome_id].push_back(gene_str);
    } else {
      // if multiple genes
      // draw random to determine whether
      // within chromosome recombination happens
      auto with_chromosome_recombination = pRandom->random_uniform();
      if (with_chromosome_recombination < config->within_chromosome_recombination_rate()) {
        // if happen draw a random crossover point based on ','
        auto cutting_gene_id = pRandom->random_uniform(f->pf_genotype_str[chromosome_id].size() - 1) + 1;
        // draw another random to do top-bottom or bottom-top cross over
        auto topOrBottom = pRandom->random_uniform();
        for (auto gene_id = 0; gene_id < cutting_gene_id; ++gene_id) {
          auto gene_str = topOrBottom < 0.5 ? f->pf_genotype_str[chromosome_id][gene_id]
                                            : m->pf_genotype_str[chromosome_id][gene_id];
          new_pf_genotype_str[chromosome_id].push_back(gene_str);
        }
        for (auto gene_id = cutting_gene_id; gene_id < f->pf_genotype_str[chromosome_id].size(); ++gene_id) {
          auto gene_str = topOrBottom < 0.5 ? m->pf_genotype_str[chromosome_id][gene_id]
                                            : f->pf_genotype_str[chromosome_id][gene_id];
          new_pf_genotype_str[chromosome_id].push_back(gene_str);
        }
      } else {
        // if there is no within chromosome recombination
        // do the same with single gene
        auto topOrBottom = pRandom->random_uniform();
        for (int gene_id = 0; gene_id < f->pf_genotype_str[chromosome_id].size(); ++gene_id) {
          auto gene_str = topOrBottom < 0.5 ? f->pf_genotype_str[chromosome_id][gene_id]
                                            : m->pf_genotype_str[chromosome_id][gene_id];

          new_pf_genotype_str[chromosome_id].push_back(gene_str);
        }
      }
    }
  }

  auto new_aa_sequence = Convert_PfGenotypeStr_To_String(new_pf_genotype_str);
  return config->genotype_db.get_genotype(new_aa_sequence, config);
}
