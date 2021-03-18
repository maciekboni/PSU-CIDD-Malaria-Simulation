//
// Created by nguyentd on 3/11/2021.
//

#include "Core/Config/Config.h"
#include "PkPdReporter.h"
#include "Events/ProgressToClinicalEvent.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Population/ImmuneSystem.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexAll.h"
#include "Reporters/Reporter.h"
#include "Spatial/Location.h"
#include "Strategies/SFTStrategy.h"
#include "Therapies/SCTherapy.h"
#include "easylogging++.h"
#include "AppInput.h"
#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <memory>

INITIALIZE_EASYLOGGINGPP

void create_cli_options(int argc, char **argv, CLI::App &app, AppInput &input);

bool validate_config(AppInput &input);

int main(int argc, char **argv) {
  AppInput input;

  CLI::App app{"Efficacy Estimator"};
  create_cli_options(argc, argv, app, input);
  CLI11_PARSE(app, argc, argv);

  if (!validate_config(input)) {
    return 1;
  }

  // Turn off logger
  el::Configurations default_conf;
  default_conf.setToDefault();
  default_conf.setGlobally(el::ConfigurationType::Enabled, "false");
  el::Loggers::reconfigureLogger("default", default_conf);
  START_EASYLOGGINGPP(argc, argv);

  auto p_model = std::make_unique<Model>();
  p_model->set_config_filename(input.input_file);

  p_model->initialize();
  // ==== override population size ======
  if (input.population_size != Model::POPULATION->size()) {
    Model::CONFIG->location_db()[0].population_size = input.population_size;
    delete Model::POPULATION;
    p_model->set_population(new Population(p_model.get()));
    Model::POPULATION = p_model->population();
    p_model->population()->initialize();
  }

  // ==== override drug type info ========

  for (int i = 0; i < input.number_of_drugs_in_combination; i++) {
    auto *dt = Model::CONFIG->drug_db()->at(i);
    dt->set_name(fmt::format("D{}", i));
    dt->set_drug_half_life(input.half_life[i]);
    dt->set_maximum_parasite_killing_rate(input.k_max[i]);
    dt->set_n(input.slope[i]);
    // TODO: add app arguments later
    dt->set_p_mutation(0.0);
    dt->set_k(4);
    Model::CONFIG->EC50_power_n_table()[0][0] = pow(input.EC50[i], dt->n());
  }

  // ======= override therapy 0 ==========
  auto scTherapy = dynamic_cast<SCTherapy *>(Model::CONFIG->therapy_db()[0]);
  scTherapy->drug_ids.clear();
  scTherapy->dosing_day.clear();

  for (int i = 0; i < input.number_of_drugs_in_combination; i++) {
    scTherapy->drug_ids.push_back(i);
    scTherapy->dosing_day.push_back(input.dosing_days[i]);
  }

  // ==========reset and override reporters ==================
  for (auto reporter : p_model->reporters()) {
    delete reporter;
  }

  p_model->reporters().clear();

  p_model->add_reporter(new PkPdReporter(&input));

  // =========infect population with genotype 0================
  auto *genotype = Model::CONFIG->genotype_db()->at(0);

  for (auto person : Model::POPULATION->all_persons()->vPerson()) {

    auto density =
        Model::CONFIG->parasite_density_level().log_parasite_density_from_liver;
    auto *blood_parasite = person->add_new_parasite_to_blood(genotype);

    person->immune_system()->set_increase(true);
    person->set_host_state(Person::EXPOSED);

    blood_parasite->set_gametocyte_level(
        Model::CONFIG->gametocyte_level_full());
    blood_parasite->set_last_update_log10_parasite_density(density);

    ProgressToClinicalEvent::schedule_event(Model::SCHEDULER, person,
                                            blood_parasite, 1);
  }

  // run model
  p_model->run();
  const auto result =
      1 - Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[0];

  fmt::print("Efficacy: {:f}\n", result);

  return 0;
}

void create_cli_options(int argc, char **argv, CLI::App &app, AppInput &input) {
  app.add_option("-i,--input", input.input_file,
                 "Input filename. Default: `input.yml`");

  app.add_option(
      "-p,--popsize", input.population_size,
      "Number of individuals used in the simulation (default: 10,000)");

  app.add_option(
      "-d,--dosing", input.dosing_days,
      "Drung dosing days.\nEx: `-d 2` or `--dosing 2` for monotherapy,\n  `-d "
      "5 2` or `--dosing 5 2` for a combination of two drugs.");

  app.add_option(
      "-t,--halflife", input.half_life,
      "Drug elimintaion half-life in date unit.\nEx: `-t 2` or "
      "`--halflife 2` for monotherapy,\n  `-t 4.5 28.0` or `--halflife 4.5 "
      "28.0` for a combination of two drugs.");

  app.add_option(
      "-k,--kmax", input.k_max,
      "The maximum fraction of parasites that can be killed per day.\nEx: `-k "
      "0.999` or `--kmax 0.999` for monotherapy,\n  `-k 0.999 0.99` or "
      "`--kmax= 0.999 0.99` for drug combination.");

  app.add_option("-e,--EC50", input.EC50,
                 "The drug concentration at which the parasite killng reach "
                 "50%.\nEx: `-e 0.75` or `--EC50 0.75` for monotherapy, \n `-e "
                 "0.75 0.65` or `--EC50 0.75 0.65` for drug combination.");

  app.add_option("-n,--slope", input.slope,
                 "the slope of the concentration-effect curve.\nEx: `-n 25` or "
                 "`--slope 25` for monotherapy,\n `-n 25 10` or `--slope 25 "
                 "10` for drug combination.");
}

bool validate_config(AppInput &input) {
  input.number_of_drugs_in_combination = input.half_life.size();

  if (input.number_of_drugs_in_combination > 5) {
    std::cerr
        << "Error: Number of drugs in combination should not greater than 5"
        << std::endl;
    return false;
  }

  if (input.k_max.size() != input.number_of_drugs_in_combination ||
      input.EC50.size() != input.number_of_drugs_in_combination ||
      input.slope.size() != input.number_of_drugs_in_combination ||
      input.dosing_days.size() != input.number_of_drugs_in_combination) {
    std::cerr << "Error: Wrong number of drugs in combination" << std::endl;
    return false;
  }

  if (input.population_size < 0) {
    std::cerr << "Error: Population size cannot be smaller than 0."
              << std::endl;
    return false;
  }

  for (auto k : input.k_max) {
    if (k >= 1 || k < 0) {
      std::cerr << "Error: k_max should be in range of (0,1]" << std::endl;
      return false;
    }
  }

  for (auto ec50 : input.EC50) {
    if (ec50 < 0) {
      std::cerr << "Error: EC50 should be greater than 0." << std::endl;
      return false;
    }
  }

  for (auto n : input.slope) {
    if (n < 0) {
      std::cerr << "Error: n should greater than 0." << std::endl;
      return false;
    }
  }

  for (auto dosing : input.dosing_days) {
    if (dosing < 0) {
      std::cerr << "Error: dosing should greater than 0." << std::endl;
      return false;
    }
  }

  std::ifstream f(input.input_file.c_str());
  if (!f.good()) {
    std::cerr << "Err: Input file error or not found!" << std::endl;
    return false;
  }

  return true;
}
