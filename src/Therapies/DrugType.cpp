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

void DrugType::populate_resistant_aa_locations(Config *config_) {
  resistant_aa_locations.clear();
  for (const auto &chromosome_info : config_->pf_genotype_info().chromosome_infos) {
    for (int gene_id = 0; gene_id < chromosome_info.gene_infos.size(); ++gene_id) {
      auto &gene_info = chromosome_info.gene_infos[gene_id];

      auto aa_pos_in_sequence = config_->pf_genotype_info().calculate_aa_pos(
          gene_info.chromosome - 1, gene_id, static_cast<int>(gene_info.aa_position_infos.size()), 0);

      for (int aa_id = 0; aa_id < gene_info.aa_position_infos.size(); ++aa_id) {
        if (gene_info.aa_position_infos[aa_id].multiplicative_effect_on_EC50.find(id_)
            != gene_info.aa_position_infos[aa_id].multiplicative_effect_on_EC50.end()) {
          resistant_aa_locations.push_back({ gene_info.chromosome - 1, gene_id, aa_id, aa_pos_in_sequence, false });
        }
        aa_pos_in_sequence++;
      }
      if (gene_info.max_copies > 1) {
        if (gene_info.cnv_multiplicative_effect_on_EC50.find(id_)
            != gene_info.cnv_multiplicative_effect_on_EC50.end()) {
          resistant_aa_locations.push_back({ gene_info.chromosome - 1, gene_id,
                                             static_cast<int>(gene_info.aa_position_infos.size()), aa_pos_in_sequence,
                                             true });
        }
      }
    }
  }
}
