#include "PkPdReporter.h"
#include "AppInput.h"
#include "Model.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexAll.h"

#include <date/date.h>

PkPdReporter::PkPdReporter(AppInput *appInput) : appInput{appInput} {}

PkPdReporter::~PkPdReporter() = default;

void PkPdReporter::initialize() {}

void PkPdReporter::before_run() {}

void PkPdReporter::begin_time_step() {
  ss << Model::SCHEDULER->current_time() << sep;

  // parasiteamia profile

  for (int i = 0; i < Model::POPULATION->all_persons()->vPerson().size(); i++) {
    // ss << i << sep;
  }

  // drug concentrate profile


  // ss << Model::POPULATION->all_persons()->vPerson().size() << sep;

  std::cout << ss.str() << std::endl;
  ss.str("");
}

void PkPdReporter::monthly_report() {}

void PkPdReporter::after_run() {}

