#include "PkPdReporter.h"

#include <date/date.h>

#include "AppInput.h"
#include "Core/Config/Config.h"
#include "Model.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/DrugsInBlood.h"
#include "Population/Person.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexAll.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Therapies/Drug.h"

PkPdReporter::PkPdReporter(AppInput* appInput) : appInput { appInput } {
  if (appInput && !appInput->output_file.empty()) {
    outputFStream.open(appInput->output_file);
  }
}

PkPdReporter::~PkPdReporter() {
  if (outputFStream.is_open()) {
    outputFStream.close();
  }
}

void PkPdReporter::initialize() {}

void PkPdReporter::before_run() {}

void PkPdReporter::begin_time_step() {
  ss << Model::SCHEDULER->current_time();

  // parasiteamia profile

  for (int i = 0; i < Model::POPULATION->all_persons()->vPerson().size(); i++) {
    auto p_person = Model::POPULATION->all_persons()->vPerson()[i];
    if (p_person->all_clonal_parasite_populations()->size() > 0) {
      ss << sep << p_person->all_clonal_parasite_populations()->parasites()->at(0)->get_log10_infectious_density();
    } else {
      ss << sep << Model::CONFIG->parasite_density_level().log_parasite_density_cured;
    }
  }

  auto start_drug_id = appInput->is_art ? 0 : 1;
  // drug concentrate profile
  for (int drug_id = start_drug_id; drug_id < appInput->number_of_drugs_in_combination + start_drug_id; drug_id++) {
    for (int i = 0; i < Model::POPULATION->all_persons()->vPerson().size(); i++) {
      auto p_person = Model::POPULATION->all_persons()->vPerson()[i];
      bool found { false };
      for (const auto& [did, active_drug] : *(p_person->drugs_in_blood()->drugs())) {
        if (drug_id == did) {
          found = true;
          ss << sep << active_drug->last_update_value();
          break;
        }
      }
      if (!found) {
        ss << sep << 0;
      }
    }
  }

  // drug killing profile
  for (int drug_id = start_drug_id; drug_id < appInput->number_of_drugs_in_combination + start_drug_id; drug_id++) {
    for (int i = 0; i < Model::POPULATION->all_persons()->vPerson().size(); i++) {
      auto p_person = Model::POPULATION->all_persons()->vPerson()[i];

      bool found { false };
      for (const auto& [did, active_drug] : *(p_person->drugs_in_blood()->drugs())) {
        if (drug_id == did) {
          found = true;
          ss << sep << active_drug->get_parasite_killing_rate(0);
          break;
        }
      }
      if (!found) {
        ss << sep << 0;
      }
    }
  }

  // ss << Model::POPULATION->all_persons()->vPerson().size() << sep;

  if (outputFStream.is_open()) {
    outputFStream << ss.str() << std::endl;
  }
  if (appInput->to_console) {
    std::cout << ss.str() << std::endl;
  }
  ss.str("");
}

void PkPdReporter::monthly_report() {}

void PkPdReporter::after_run() {}
