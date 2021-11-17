//
// Created by nguyentd on 11/2/2021.
//

#include "IntroduceParasitesPeriodicallyEventV2.h"
#include "Model.h"
#include "Population/Population.h"
#include "Population/ImmuneSystem.h"
#include "Population/ClonalParasitePopulation.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "MDC/ModelDataCollector.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include <easylogging++.h>

OBJECTPOOL_IMPL(IntroduceParasitesPeriodicallyEventV2)

IntroduceParasitesPeriodicallyEventV2::IntroduceParasitesPeriodicallyEventV2(
    const std::vector<std::vector<double>>& allele_distributions_in,
    const int& location, const int& duration,
    const int& number_of_cases, const int& start_day_in, const int & end_day_in
)
    : allele_distributions(allele_distributions_in),
      location_(location), duration_(duration), number_of_cases_(number_of_cases),
      start_day(start_day_in), end_day(end_day_in) {

  time = start_day;

  if (end_day_in == -1){
    end_day = Model::CONFIG->total_time();
  }
}

IntroduceParasitesPeriodicallyEventV2::~IntroduceParasitesPeriodicallyEventV2() = default;

void IntroduceParasitesPeriodicallyEventV2::schedule_event(
    Scheduler* scheduler, IntroduceParasitesPeriodicallyEventV2* old_event
) {
  if (scheduler != nullptr) {
    auto* e = new IntroduceParasitesPeriodicallyEventV2(
        old_event->allele_distributions,
        old_event->location(), old_event->duration(),
        old_event->number_of_cases(), old_event->start_day, old_event->end_day
    );
    e->dispatcher = nullptr;
    e->time = scheduler->current_time() + 1;
    scheduler->schedule_population_event(e);
  }
}

void IntroduceParasitesPeriodicallyEventV2::execute() {
  // std::cout << date::year_month_day{ Model::SCHEDULER->calendar_date } << ":import periodically event" << std::endl;
  //schedule importation for the next day
  if (Model::SCHEDULER->current_time() < end_day) {
    schedule_event(Model::SCHEDULER, this);
  }
//  else {
//    LOG(INFO) << "Hello End importation" ;
//  }

  const auto number_of_importation_cases = Model::RANDOM->random_poisson(
      static_cast<double>(number_of_cases_) / duration_
  );
  if (Model::DATA_COLLECTOR->popsize_by_location_hoststate()[location_][0] < number_of_importation_cases) {
    return;
  }

  //    std::cout << number_of_cases_ << std::endl;
  auto* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  VLOG_IF(number_of_importation_cases > 0, 2) << "Day: " << Model::SCHEDULER->current_time() << " - Importing "
                                              << number_of_importation_cases
                                              << " at location " << location_;

  for (auto i = 0; i < number_of_importation_cases; i++) {

    std::size_t ind_ac = Model::RANDOM->random_uniform(static_cast<unsigned long>(pi->vPerson()[location_][0].size()));
    if (pi->vPerson()[location_][0][ind_ac].empty()) {
      continue;
    }

    std::size_t index = Model::RANDOM->random_uniform(pi->vPerson()[location_][0][ind_ac].size());
    auto* p = pi->vPerson()[location_][0][ind_ac][index];

    p->immune_system()->set_increase(true);
    p->set_host_state(Person::ASYMPTOMATIC);

    //check and draw random Genotype
    std::vector<int> gene_structure(allele_distributions.size());

    for (int j = 0; j < allele_distributions.size(); ++j) {
      int k = 0;
      double sum = allele_distributions[j][k];
      double r = Model::RANDOM->random_uniform();

      while (r > sum) {
        k += 1;
        sum += allele_distributions[j][k];
      }
      gene_structure[j] = k;
    }

    Genotype* imported_genotype = Model::CONFIG->genotype_db()->get_genotype_from_alleles_structure(gene_structure);

    auto* blood_parasite = p->add_new_parasite_to_blood(imported_genotype);

    auto size = Model::CONFIG->parasite_density_level().log_parasite_density_asymptomatic;

    blood_parasite->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
    blood_parasite->set_last_update_log10_parasite_density(size);
    blood_parasite->set_update_function(Model::MODEL->immunity_clearance_update_function());

  }
}