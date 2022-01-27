/*
 * File:   DrugType.cpp
 * Author: nguyentran
 *
 * Created on June 3, 2013, 10:59 AM
 */

#include "DrugType.h"

#include <cmath>

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Model.h"
#include "Parasites/Genotype.h"
#include "easylogging++.h"

#ifndef LOG2_10
#define LOG2_10 3.32192809489
#endif

DrugType::DrugType()
    : id_(0),
      drug_half_life_(0),
      maximum_parasite_killing_rate_(0),
      p_mutation_(0),
      k_(0),
      cut_off_percent_(0),
      n_(1) {}

DrugType::~DrugType() = default;

double DrugType::get_parasite_killing_rate_by_concentration(const double &concentration, const double &EC50_power_n) {
  const auto con_power_n = pow(concentration, n_);
  return maximum_parasite_killing_rate_ * con_power_n / (con_power_n + EC50_power_n);
}

double DrugType::n() {
  return n_;
}

void DrugType::set_n(const double &n) {
  n_ = n;
  //    set_EC50_power_n(pow(EC50_, n_));
}

int DrugType::get_total_duration_of_drug_activity(const int &dosing_days) const {
  // CutOffPercent is 10
  // log2(100.0 / 10.0) = 3.32192809489
  return dosing_days + ceil(drug_half_life_ * LOG2_10);
}

double DrugType::infer_ec50(Genotype *genotype) {
  // TODO: rework on this

  return 0.65;

  assert(false);
  el::Logger *defaultLogger = el::Loggers::getLogger("default");
  defaultLogger->fatal("EC50 not match for genotype: %s", *genotype);
  // hopefully it will never reach here
  return 0;
}

void DrugType::populate_resistant_aa_locations(Config *config_) {
  resistant_aa_locations.clear();
  for (const auto &chromosome_info : config_->pf_gene_info().chromosome_infos) {
    for (int gene_id = 0; gene_id < chromosome_info.gene_infos.size(); ++gene_id) {
      auto &gene_info = chromosome_info.gene_infos[gene_id];

      auto aa_pos_in_sequence = config_->pf_gene_info().calculate_aa_pos(
          gene_info.chromosome - 1, gene_id, static_cast<int>(gene_info.aa_position_infos.size()), 0);

      for (int aa_id = 0; aa_id < gene_info.aa_position_infos.size(); ++aa_id) {
        if (config_->mutation_mask()[aa_pos_in_sequence] == '1'
            && gene_info.aa_position_infos[aa_id].multiplicative_effect_on_EC50.find(id_)
                   != gene_info.aa_position_infos[aa_id].multiplicative_effect_on_EC50.end()) {
          resistant_aa_locations.push_back({ gene_info.chromosome - 1, gene_id, aa_id, aa_pos_in_sequence, false });
        }
        aa_pos_in_sequence++;
      }
      if (gene_info.max_copies > 1) {
        if (config_->mutation_mask()[aa_pos_in_sequence] == '1'
            && gene_info.cnv_multiplicative_effect_on_EC50.find(id_)
                   != gene_info.cnv_multiplicative_effect_on_EC50.end()) {
          resistant_aa_locations.push_back({ gene_info.chromosome - 1, gene_id,
                                             static_cast<int>(gene_info.aa_position_infos.size()), aa_pos_in_sequence,
                                             true });
        }
      }
    }
  }
  //  for (auto gene : resistant_genes) {
  //    auto found = false;
  //
  //    for (const auto &chromosome_info : config_->pf_gene_info().chromosome_infos) {
  //      auto gene_info_it = std::find_if(chromosome_info.gene_infos.begin(), chromosome_info.gene_infos.end(),
  //                                       [&gene](const GeneInfo &gene_info) { return gene_info.name == gene; });
  //      if (gene_info_it != chromosome_info.gene_infos.end()) {
  //        auto aa_pos_in_sequence = config_->pf_gene_info().calculate_aa_pos(
  //            (*gene_info_it).chromosome - 1,
  //            static_cast<int>(std::distance(chromosome_info.gene_infos.begin(), gene_info_it)),
  //            static_cast<int>((*gene_info_it).aa_position_infos.size()), 0);
  //        for (int aa_id = 0; aa_id < (*gene_info_it).aa_position_infos.size(); ++aa_id) {
  //          if (config_->mutation_mask()[aa_pos_in_sequence] == '1') {
  //            resistant_aa_locations.push_back(
  //                { (*gene_info_it).chromosome - 1,
  //                  static_cast<int>(std::distance(chromosome_info.gene_infos.begin(), gene_info_it)), aa_id,
  //                  aa_pos_in_sequence, false });
  //          }
  //          aa_pos_in_sequence++;
  //        }
  //        if ((*gene_info_it).max_copies > 1) {
  //          if (config_->mutation_mask()[aa_pos_in_sequence] == '1') {
  //            resistant_aa_locations.push_back(
  //                { (*gene_info_it).chromosome - 1,
  //                  static_cast<int>(std::distance(chromosome_info.gene_infos.begin(), gene_info_it)),
  //                  static_cast<int>((*gene_info_it).aa_position_infos.size()), aa_pos_in_sequence, true });
  //          }
  //        }
  //
  //        found = true;
  //        break;
  //      }
  //    }
  //    if (!found) {
  //      LOG(FATAL) << "Incorrect resistant genes in Drug DB";
  //    }
  //  }
}
