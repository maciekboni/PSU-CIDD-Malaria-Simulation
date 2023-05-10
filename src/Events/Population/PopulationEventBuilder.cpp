#define NOMINMAX

#include "PopulationEventBuilder.h"

#include <Model.h>

#include <algorithm>
#include <vector>

#include "ChangeInterruptedFeedingRateEvent.h"
#include "ChangeStrategyEvent.h"
#include "ChangeTreatmentCoverageEvent.h"
#include "ChangeWithinHostInducedRecombinationEvent.h"
#include "Core/Config/Config.h"
#include "ImportationEvent.h"
#include "ImportationPeriodicallyEvent.h"
#include "Introduce580YMutantEvent.h"
#include "IntroduceAQMutantEvent.h"
#include "IntroduceLumefantrineMutantEvent.h"
#include "IntroduceParasitesPeriodicallyEventV2.h"
#include "IntroducePlas2CopyParasiteEvent.h"
#include "IntroduceTripleMutantToDPMEvent.h"
#include "ModifyNestedMFTEvent.h"
#include "SingleRoundMDAEvent.h"
#include "TurnOffMutationEvent.h"
#include "TurnOnMutationEvent.h"
#include "yaml-cpp/yaml.h"

std::vector<Event*> PopulationEventBuilder::build_introduce_parasite_events(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    auto location = event_node["location"].as<int>();
    if (location < config->number_of_locations()) {
      for (std::size_t j = 0; j < event_node["parasite_info"].size(); j++) {
        auto genotype_id = event_node["parasite_info"][j]["genotype_id"].as<int>();
        auto num = event_node["parasite_info"][j]["number_of_cases"].as<int>();

        const auto starting_date = event_node["parasite_info"][j]["day"].as<date::year_month_day>();
        auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();

        auto* event = new ImportationEvent(location, time, genotype_id, num);
        events.push_back(event);
      }
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_parasites_periodically_events(const YAML::Node& node,
                                                                                          Config* config) {
  std::vector<Event*> events;

  for (const auto& event_node : node) {
    const auto location = event_node["location"].as<unsigned long>();
    const auto location_from = location == -1 ? 0 : location;
    const auto location_to =
        location == -1 ? config->number_of_locations() : std::min(location + 1, config->number_of_locations());

    for (auto loc = location_from; loc < location_to; ++loc) {
      for (std::size_t j = 0; j < event_node["parasite_info"].size(); j++) {
        const auto genotype_id = event_node["parasite_info"][j]["genotype_id"].as<int>();
        const auto dur = event_node["parasite_info"][j]["duration"].as<int>();
        const auto num = event_node["parasite_info"][j]["number_of_cases"].as<int>();

        const auto starting_date = event_node["parasite_info"][j]["start_day"].as<date::year_month_day>();
        auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();

        auto* event = new ImportationPeriodicallyEvent(loc, dur, genotype_id, num, time);
        events.push_back(event);
      }
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_parasites_periodically_events_v2(const YAML::Node& node,
                                                                                             Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    const auto location = event_node["location"].as<unsigned long>();
    const auto location_from = location == -1 ? 0 : location;
    const auto location_to =
        location == -1 ? config->number_of_locations() : std::min(location + 1, config->number_of_locations());

    for (auto loc = location_from; loc < location_to; ++loc) {
      for (auto j = 0; j < event_node["parasite_info"].size(); j++) {
        const auto dur = event_node["parasite_info"][j]["duration"].as<int>();
        const auto num = event_node["parasite_info"][j]["number_of_cases"].as<int>();

        const auto starting_date = event_node["parasite_info"][j]["start_day"].as<date::year_month_day>();

        date::year_month_day end_date = Model::CONFIG->ending_date();

        if (event_node["parasite_info"][j]["end_day"]) {
          end_date = event_node["parasite_info"][j]["end_day"].as<date::year_month_day>();
        }

        auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();
        auto end_time = (date::sys_days { end_date } - date::sys_days { config->starting_date() }).count();

        // TODO: rework this with new genotype implementation
        //        std::vector<std::vector<double>>
        //        allele_distributions(Model::CONFIG->genotype_info().loci_vector.size());
        // generate default distributions
        //        for (int k = 0; k < Model::CONFIG->genotype_info().loci_vector.size(); ++k) {
        //          auto number_of_alleles = Model::CONFIG->genotype_info().loci_vector[k].alleles.size();
        //          for (int l = 0; l < number_of_alleles; ++l) {
        //            allele_distributions[k].push_back(1.0/number_of_alleles);
        //          }
        //        }

        // read and replace
        //        for (auto m = 0; m < node[i]["parasite_info"][j]["allele_distributions"].size(); m++) {
        //          auto pos = node[i]["parasite_info"][j]["allele_distributions"][m]["position"].as<int>();
        //          for (int n = 0; n < node[i]["parasite_info"][j]["allele_distributions"][m]["distribution"].size();
        //          ++n) {
        //            allele_distributions[pos][n] =
        //            node[i]["parasite_info"][j]["allele_distributions"][m]["distribution"][n].as<double>();
        //          }
        //        }
        //
        //        auto* event = new IntroduceParasitesPeriodicallyEventV2(allele_distributions, loc, dur, num, time,
        //        end_time); events.push_back(event);
      }
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_change_treatment_coverage_event(const YAML::Node& node,
                                                                                  Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    auto* tcm = ITreatmentCoverageModel::build(event_node, config);
    // std::cout << tcm->starting_time << std::endl;
    auto* e = new ChangeTreatmentCoverageEvent(tcm);
    events.push_back(e);
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_change_treatment_strategy_event(const YAML::Node& node,
                                                                                  Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    const auto starting_date = event_node["day"].as<date::year_month_day>();
    auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();
    auto strategy_id = event_node["strategy_id"].as<int>();

    auto* e = new ChangeStrategyEvent(time, strategy_id);
    events.push_back(e);
  }

  return events;
}

std::vector<Event*> PopulationEventBuilder::build_single_round_mda_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    const auto starting_date = event_node["day"].as<date::year_month_day>();
    auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();
    auto* e = new SingleRoundMDAEvent(time);
    for (std::size_t loc = 0; loc < config->number_of_locations(); loc++) {
      auto input_loc = event_node["fraction_population_targeted"].size() < config->number_of_locations() ? 0 : loc;
      e->fraction_population_targeted.push_back(event_node["fraction_population_targeted"][input_loc].as<double>());
    }

    e->days_to_complete_all_treatments = event_node["days_to_complete_all_treatments"].as<int>();
    events.push_back(e);
  }

  return events;
}

std::vector<Event*> PopulationEventBuilder::build_modify_nested_mft_strategy_event(const YAML::Node& node,
                                                                                   Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    const auto starting_date = event_node["day"].as<date::year_month_day>();
    auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();
    auto strategy_id = event_node["strategy_id"].as<int>();

    auto* e = new ModifyNestedMFTEvent(time, strategy_id);
    events.push_back(e);
  }

  return events;
}

std::vector<Event*> PopulationEventBuilder::build_turn_on_mutation_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    const auto starting_date = event_node["day"].as<date::year_month_day>();
    auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();
    double mutation_probability = event_node["mutation_probability"] ? event_node["mutation_probability"].as<double>()
                                                                     : Model::CONFIG->mutation_probability_by_locus();

    auto* e = new TurnOnMutationEvent(time, mutation_probability);
    events.push_back(e);
  }

  return events;
}

std::vector<Event*> PopulationEventBuilder::build_turn_off_mutation_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    const auto starting_date = event_node["day"].as<date::year_month_day>();
    auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();
    auto* e = new TurnOffMutationEvent(time);
    events.push_back(e);
  }

  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_plas2_parasite_events(const YAML::Node& node,
                                                                                  Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    auto location = event_node["location"].as<int>();
    if (location < config->number_of_locations()) {
      auto fraction = event_node["fraction"].as<double>();

      const auto starting_date = event_node["day"].as<date::year_month_day>();
      auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();

      auto* event = new IntroducePlas2CopyParasiteEvent(location, time, fraction);
      events.push_back(event);
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_580Y_mutant_events(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    auto location = event_node["location"].as<int>();
    if (location < config->number_of_locations()) {
      auto fraction = event_node["fraction"].as<double>();

      const auto starting_date = event_node["day"].as<date::year_month_day>();
      auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();

      auto* event = new Introduce580YMutantEvent(location, time, fraction);
      events.push_back(event);
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_aq_mutant_parasite_events(const YAML::Node& node,
                                                                                      Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    auto location = event_node["location"].as<int>();
    if (location < config->number_of_locations()) {
      auto fraction = event_node["fraction"].as<double>();

      const auto starting_date = event_node["day"].as<date::year_month_day>();
      auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();

      auto* event = new IntroduceAQMutantEvent(location, time, fraction);
      events.push_back(event);
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_lumefantrine_mutant_parasite_events(const YAML::Node& node,
                                                                                                Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    auto location = event_node["location"].as<int>();
    if (location < config->number_of_locations()) {
      auto fraction = event_node["fraction"].as<double>();

      const auto starting_date = event_node["day"].as<date::year_month_day>();
      auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();

      auto* event = new IntroduceLumefantrineMutantEvent(location, time, fraction);
      events.push_back(event);
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_triple_mutant_to_dpm_parasite_events(const YAML::Node& node,
                                                                                                 Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    auto location = event_node["location"].as<int>();
    if (location < config->number_of_locations()) {
      auto fraction = event_node["fraction"].as<double>();

      const auto starting_date = event_node["day"].as<date::year_month_day>();
      auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();

      auto* event = new IntroduceTrippleMutantToDPMEvent(location, time, fraction);
      events.push_back(event);
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_change_interrupted_feeding_rate_event(const YAML::Node& node,
                                                                                        Config* config) {
  std::vector<Event*> events;
  for (const auto& event_node : node) {
    auto location = event_node["location"].as<int>();
    if (location < config->number_of_locations()) {
      const auto starting_date = event_node["day"].as<date::year_month_day>();
      auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();
      auto ifr = event_node["rate"].as<double>();
      auto* event = new ChangeInterruptedFeedingRateEvent(location, ifr, time);
      events.push_back(event);
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_change_within_host_induced_recombination(const YAML::Node event_node,
                                                                                           Config* config) {
  std::vector<Event*> events;
  const auto starting_date = event_node["day"].as<date::year_month_day>();
  auto time = (date::sys_days { starting_date } - date::sys_days { config->starting_date() }).count();
  auto value = event_node["value"].as<bool>();
  auto* event = new ChangeWithinHostInducedRecombinationEvent(value, time);
  events.push_back(event);

  return events;
}

std::vector<Event*> PopulationEventBuilder::build(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  const auto name = node["name"].as<std::string>();
  // std::cout << name << std::endl;
  if (name == "introduce_plas2_parasites") {
    events = build_introduce_plas2_parasite_events(node["info"], config);
  }
  if (name == "introduce_580Y_parasites") {
    events = build_introduce_580Y_mutant_events(node["info"], config);
  }
  if (name == "introduce_aq_mutant_parasites") {
    events = build_introduce_aq_mutant_parasite_events(node["info"], config);
  }

  if (name == "introduce_lumefantrine_mutant_parasites") {
    events = build_introduce_lumefantrine_mutant_parasite_events(node["info"], config);
  }

  if (name == "introduce_triple_mutant_to_dpm_parasites") {
    events = build_introduce_triple_mutant_to_dpm_parasite_events(node["info"], config);
  }

  if (name == "introduce_parasites") {
    events = build_introduce_parasite_events(node["info"], config);
  }
  if (name == "introduce_parasites_periodically") {
    events = build_introduce_parasites_periodically_events(node["info"], config);
  }

  if (name == "introduce_parasites_periodically_v2") {
    events = build_introduce_parasites_periodically_events_v2(node["info"], config);
  }

  if (name == "change_treatment_coverage") {
    events = build_change_treatment_coverage_event(node["info"], config);
  }

  if (name == "change_treatment_strategy") {
    events = build_change_treatment_strategy_event(node["info"], config);
  }

  if (name == "single_round_MDA") {
    events = build_single_round_mda_event(node["info"], config);
  }

  if (name == "modify_nested_mft_strategy") {
    events = build_modify_nested_mft_strategy_event(node["info"], config);
  }
  if (name == "turn_on_mutation") {
    events = build_turn_on_mutation_event(node["info"], config);
  }
  if (name == "turn_off_mutation") {
    events = build_turn_off_mutation_event(node["info"], config);
  }
  if (name == "change_interrupted_feeding_rate") {
    events = build_change_interrupted_feeding_rate_event(node["info"], config);
  }
  if (name == "change_within_host_induced_recombination") {
    events = build_change_within_host_induced_recombination(node["info"], config);
  }
  return events;
}
