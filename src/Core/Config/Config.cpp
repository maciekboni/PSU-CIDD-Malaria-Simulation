/*
 * File:   Config.cpp
 * Author: nguyentran
 *
 * Created on March 27, 2013, 10:38 AM
 */

#include "Config.h"

#include <fmt/format.h>

#include <cmath>
#include <fstream>

#include "Core/Random.h"
#include "Helpers/OSHelpers.h"
#include "Helpers/StringHelpers.h"
#include "Model.h"
#include "Therapies/SCTherapy.h"
#include "easylogging++.h"

using namespace Spatial;

Config::Config(Model *model)
    : model_(model),
      modified_mutation_factor_(-1),
      modified_drug_half_life_(-1),
      modified_daily_cost_of_resistance_(-1),
      modified_mutation_probability_(-1) {}

Config::~Config() {
//  for (auto *therapy : therapy_db()) {
//    delete therapy;
//  }

//  for (auto *strategy : strategy_db()) {
//    delete strategy;
//  }

//  for (auto *event : preconfig_population_events()) {
//    delete event;
//  }

};

void Config::read_from_file(const std::string &config_file_name) {
  YAML::Node config;
  try {
    config = YAML::LoadFile(config_file_name);
  } catch (YAML::BadFile &ex) {
    LOG(FATAL) << config_file_name << " not found or err... Ex: " << ex.msg;
  } catch (YAML::Exception &ex) {
    LOG(FATAL) << "error: " << ex.msg << " at line " << ex.mark.line + 1 << ":" << ex.mark.column + 1;
  }

  for (auto &config_item : config_items) {
    LOG(INFO) << "Reading config item: " << config_item->name();
    config_item->set_value(config);
  }
}

// TODO: call this function in turn on/off mutation event
void Config::update_mutation_mask(const std::string &new_mask) {
  mutation_mask() = new_mask;
  for (auto [drug_id, dt] : *drug_db()) {
    dt->populate_resistant_aa_locations(this);
  }
}
