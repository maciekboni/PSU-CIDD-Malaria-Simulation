/*
 * File:   IndividualsFileReporter.cpp
 * Author: Merlin
 *
 * Created on November 7, 2014, 11:06 AM
 */

#include "IndividualsFileReporter.h"

#include "Core/Config/Config.h"
#include "Core/Scheduler.h"
#include "Model.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/Person.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexAll.h"
#include "Population/SingleHostClonalParasitePopulations.h"

IndividualsFileReporter::IndividualsFileReporter(const std::string &file_name)
    : file_name_ { file_name } {}

IndividualsFileReporter::~IndividualsFileReporter() = default;

void IndividualsFileReporter::initialize() {}

void IndividualsFileReporter::before_run() {
  fs_.open(file_name_.c_str(), std::fstream::out);
}

void IndividualsFileReporter::begin_time_step() {
  auto persons = Model::POPULATION->all_persons()->vPerson();
  for (size_t i = 0; i < persons.size(); i++) {
    double p_density = 0;
    if (!persons[i]->all_clonal_parasite_populations()->parasites()->empty()) {
      p_density = persons[i]
                      ->all_clonal_parasite_populations()
                      ->parasites()
                      ->at(0)
                      ->last_update_log10_parasite_density();
    } else {
      p_density =
          Model::CONFIG->parasite_density_level().log_parasite_density_cured;
    }
    fs_ << p_density;
    if (i != persons.size() - 1) {
      fs_ << ", ";
    }
  }
  fs_ << std::endl;
}

void IndividualsFileReporter::after_time_step() {}

void IndividualsFileReporter::monthly_report() {}

void IndividualsFileReporter::after_run() {
  fs_.close();
}
