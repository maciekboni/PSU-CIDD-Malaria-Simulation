/*
 * File:   Population.cpp
 * Author: nguyentran
 *
 * Created on April 15, 2013, 10:49 AM
 */

#include "Population.h"

#include <cfloat>
#include <cmath>

#include "Constants.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Events/BirthdayEvent.h"
#include "Events/SwitchImmuneComponentEvent.h"
#include "Helpers/ObjectHelpers.h"
#include "Helpers/TimeHelpers.h"
#include "ImmuneSystem.h"
#include "InfantImmuneComponent.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Mosquito/Mosquito.h"
#include "NonInfantImmuneComponent.h"
#include "Properties/PersonIndexAll.h"
#include "Properties/PersonIndexByLocationMovingLevel.h"
#include "Properties/PersonIndexByLocationStateAgeClass.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Spatial/SpatialModel.h"
#include "easylogging++.h"

Population::Population(Model* model) : model_(model) {
  person_index_list_ = new PersonIndexPtrList();
  all_persons_ = new PersonIndexAll();

  person_index_list_->push_back(all_persons_);
}

Population::~Population() {
  // release memory for all persons
  if (all_persons_ != nullptr) {
    for (auto& person : all_persons_->vPerson()) {
      ObjectHelpers::delete_pointer<Person>(person);
    }
    all_persons_->vPerson().clear();
    all_persons_ = nullptr;
  }

  // release person_indexes

  if (person_index_list_ != nullptr) {
    for (PersonIndex* person_index : *person_index_list_) {
      ObjectHelpers::delete_pointer<PersonIndex>(person_index);
    }

    person_index_list_->clear();
    ObjectHelpers::delete_pointer<PersonIndexPtrList>(person_index_list_);
  }
}

void Population::add_person(Person* person) {
  for (PersonIndex* person_index : *person_index_list_) {
    person_index->add(person);
  }
  person->set_population(this);
}

void Population::remove_person(Person* person) {
  for (PersonIndex* person_index : *person_index_list_) {
    person_index->remove(person);
  }
  person->set_population(nullptr);
}

void Population::remove_dead_person(Person* person) {
  remove_person(person);
  ObjectHelpers::delete_pointer<Person>(person);
}

void Population::notify_change(Person* p, const Person::Property& property, const void* oldValue,
                               const void* newValue) {
  for (PersonIndex* person_index : *person_index_list_) {
    person_index->notify_change(p, property, oldValue, newValue);
  }
}

std::size_t Population::size(const int& location, const int& age_class) {
  if (location == -1) {
    return all_persons_->size();
  }
  auto pi_lsa = get_person_index<PersonIndexByLocationStateAgeClass>();

  if (pi_lsa == nullptr) {
    return 0;
  }
  std::size_t temp = 0;
  if (age_class == -1) {
    for (auto state = 0; state < Person::NUMBER_OF_STATE - 1; state++) {
      for (auto ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        temp += pi_lsa->vPerson()[location][state][ac].size();
      }
    }
  } else {
    for (auto state = 0; state < Person::NUMBER_OF_STATE - 1; state++) {
      temp += pi_lsa->vPerson()[location][state][age_class].size();
    }
  }
  return temp;
}

std::size_t Population::size(const int& location, const Person::HostStates& hs, const int& age_class) {
  if (location == -1) {
    return all_persons_->size();
  }
  auto pi_lsa = get_person_index<PersonIndexByLocationStateAgeClass>();
  return (pi_lsa->vPerson()[location][hs][age_class].size());
}

// new
std::size_t Population::size_residents_only(const int& location) {
  if (location == -1) {
    return all_persons_->size();
  }

  auto pi_lsa = get_person_index<PersonIndexByLocationStateAgeClass>();

  if (pi_lsa == nullptr) {
    return 0;
  }
  auto temp = 0ul;
  for (auto state = 0; state < Person::NUMBER_OF_STATE - 1; state++) {
    for (auto ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      for (auto i = 0; i < pi_lsa->vPerson()[location][state][ac].size(); i++) {
        if (pi_lsa->vPerson()[location][state][ac][i]->residence_location() == location) {
          temp++;
        }
      }
    }
  }
  return temp;
}

void Population::perform_infection_event() {
  //    std::cout << "Infection Event" << std::endl;

  PersonPtrVector today_infections;
  auto tracking_index = Model::SCHEDULER->current_time() % Model::CONFIG->number_of_tracking_days();
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    const auto force_of_infection = force_of_infection_for_N_days_by_location[tracking_index][loc];
    if (force_of_infection <= DBL_EPSILON) continue;

    const auto new_beta = Model::CONFIG->location_db()[loc].beta
                          * Model::MODEL->get_seasonal_factor(Model::SCHEDULER->calendar_date, loc);

    auto poisson_means = new_beta * force_of_infection;

    auto number_of_bites = Model::RANDOM->random_poisson(poisson_means);
    if (number_of_bites <= 0) continue;

    // data_collector store number of bites
    Model::DATA_COLLECTOR->collect_number_of_bites(loc, number_of_bites);

    auto persons_bitten_today = Model::RANDOM->roulette_sampling<Person>(
        number_of_bites, individual_relative_biting_by_location[loc], all_alive_persons_by_location[loc], false,
        sum_relative_biting_by_location[loc]);

    for (auto* person : persons_bitten_today) {
      assert(person->host_state() != Person::DEAD);
      person->increase_number_of_times_bitten();

      auto genotype_id = Model::MOSQUITO->random_genotype(loc, tracking_index);

      const auto p_infectious = Model::RANDOM->random_flat(0.0, 1.0);
      // only infect with real infectious bite
      if (Model::CONFIG->using_variable_probability_infectious_bites_cause_infection()) {
        if (p_infectious <= person->p_infection_from_an_infectious_bite()) {
          if (person->host_state() != Person::EXPOSED && person->liver_parasite_type() == nullptr) {
            person->today_infections()->push_back(genotype_id);
            today_infections.push_back(person);
          }
        }
      } else if (p_infectious <= Model::CONFIG->p_infection_from_an_infectious_bite()) {
        if (person->host_state() != Person::EXPOSED && person->liver_parasite_type() == nullptr) {
          person->today_infections()->push_back(genotype_id);
          today_infections.push_back(person);
        }
      }
    }
  }
  //    std::cout << "Solve infections"<< std::endl;
  // solve Multiple infections
  if (today_infections.empty()) return;

  for (auto* p : today_infections) {
    if (!p->today_infections()->empty()) {
      Model::DATA_COLLECTOR->monthly_number_of_new_infections_by_location()[p->location()] += 1;
    }
    p->randomly_choose_parasite();
  }

  today_infections.clear();
}

void Population::initialize() {
  if (model() != nullptr) {
    // those vector will be used in the initial infection
    const auto number_of_locations = Model::CONFIG->number_of_locations();

    individual_relative_biting_by_location =
        std::vector<std::vector<double>>(number_of_locations, std::vector<double>());
    individual_relative_moving_by_location =
        std::vector<std::vector<double>>(number_of_locations, std::vector<double>());
    individual_foi_by_location = std::vector<std::vector<double>>(number_of_locations, std::vector<double>());

    all_alive_persons_by_location = std::vector<std::vector<Person*>>(number_of_locations, std::vector<Person*>());

    sum_relative_biting_by_location = std::vector<double>(number_of_locations, 0);
    sum_relative_moving_by_location = std::vector<double>(number_of_locations, 0);

    current_force_of_infection_by_location = std::vector<double>(number_of_locations, 0);

    force_of_infection_for_N_days_by_location = std::vector<std::vector<double>>(
        Model::CONFIG->number_of_tracking_days(), std::vector<double>(number_of_locations, 0));

    // initalize other person index
    initialize_person_indices();

    // initialize population
    for (auto loc = 0; loc < number_of_locations; loc++) {
      const auto popsize_by_location = static_cast<int>(Model::CONFIG->location_db()[loc].population_size
                                                        * Model::CONFIG->artificial_rescaling_of_population_size());
      auto temp_sum = 0;
      for (auto age_class = 0; age_class < Model::CONFIG->initial_age_structure().size(); age_class++) {
        auto number_of_individual_by_loc_ageclass = 0;
        if (age_class == Model::CONFIG->initial_age_structure().size() - 1) {
          number_of_individual_by_loc_ageclass = popsize_by_location - temp_sum;
        } else {
          number_of_individual_by_loc_ageclass =
              static_cast<int>(popsize_by_location * Model::CONFIG->location_db()[loc].age_distribution[age_class]);
          temp_sum += number_of_individual_by_loc_ageclass;
        }

        //                std::cout << loc << "\t" << age_class << "\t" << number_of_individual_by_loc_ageclass <<
        //                std::endl;
        for (auto i = 0; i < number_of_individual_by_loc_ageclass; i++) {
          auto p = new Person();
          p->init();

          p->set_location(loc);
          p->set_residence_location(loc);
          p->set_host_state(Person::SUSCEPTIBLE);

          const auto age_from = (age_class == 0) ? 0 : Model::CONFIG->initial_age_structure()[age_class - 1];
          const auto age_to = Model::CONFIG->initial_age_structure()[age_class];

          // std::cout << i << "\t" << age_class << "\t" << age_from << "\t" << age_to << std::endl;

          // set age will also set ageclass
          p->set_age(static_cast<const int&>(Model::RANDOM->random_uniform_int(age_from, age_to + 1)));
          //                    std::cout << p->age() << " \t" << p->age_class() << std::endl;
          //                    p->set_age_class(age_class);

          int days_to_next_birthday = Model::RANDOM->random_uniform(Constants::DAYS_IN_YEAR());

          auto simulation_time_birthday = TimeHelpers::get_simulation_time_birthday(days_to_next_birthday, p->age(),
                                                                                    Model::SCHEDULER->calendar_date);
          p->set_birthday(simulation_time_birthday);

          LOG_IF(simulation_time_birthday > 0, FATAL)
              << "simulation_time_birthday have to be <= 0 when initilizing population";

          BirthdayEvent::schedule_event(Model::SCHEDULER, p, days_to_next_birthday);

          // set immune component
          if (simulation_time_birthday + Constants::DAYS_IN_YEAR() / 2 >= 0) {
            LOG_IF(p->age() > 0, FATAL) << "Error in calculating simulation_time_birthday";
            // LOG(INFO) << "Infant: " << p->age() << " - " << simulation_time_birthday;
            p->immune_system()->set_immune_component(new InfantImmuneComponent());
            // schedule for switch
            SwitchImmuneComponentEvent::schedule_for_switch_immune_component_event(
                Model::SCHEDULER, p, simulation_time_birthday + Constants::DAYS_IN_YEAR() / 2);
          } else {
            // LOG(INFO) << "Adult: " << p->age() << " - " << simulation_time_birthday;
            p->immune_system()->set_immune_component(new NonInfantImmuneComponent());
          }

          auto immune_value = Model::RANDOM->random_beta(Model::CONFIG->immune_system_information().alpha_immune,
                                                         Model::CONFIG->immune_system_information().beta_immune);
          p->immune_system()->immune_component()->set_latest_value(immune_value);
          p->immune_system()->set_increase(false);
          //                    p->draw_random_immune();

          p->innate_relative_biting_rate = Model::RANDOM->random_gamma(Model::CONFIG->relative_bitting_info().gamma_a,
                                                                       Model::CONFIG->relative_bitting_info().gamma_b);
          p->update_relative_bitting_rate();

          p->set_moving_level(Model::CONFIG->moving_level_generator().draw_random_level(Model::RANDOM));

          p->set_latest_update_time(0);

          int time = Model::RANDOM->random_uniform(Model::CONFIG->update_frequency()) + 1;
          p->generate_prob_present_at_mda_by_age();

          add_person(p);

          individual_relative_biting_by_location[loc].push_back(p->current_relative_biting_rate);
          individual_relative_moving_by_location[loc].push_back(
              Model::CONFIG->circulation_info().v_moving_level_value[p->moving_level()]);

          sum_relative_biting_by_location[loc] += p->current_relative_biting_rate;
          sum_relative_moving_by_location[loc] +=
              Model::CONFIG->circulation_info().v_moving_level_value[p->moving_level()];

          all_alive_persons_by_location[loc].push_back(p);
        }
      }
    }
  }
}

void Population::introduce_initial_cases() {
  if (model_ != nullptr) {
    // std::cout << Model::CONFIG->initial_parasite_info().size() << std::endl;
    for (const auto p_info : Model::CONFIG->initial_parasite_info()) {
      auto num_of_infections = Model::RANDOM->random_poisson(std::round(size(p_info.location) * p_info.prevalence));
      num_of_infections = num_of_infections <= 0 ? 1 : num_of_infections;

      auto* genotype = Model::CONFIG->genotype_db.at(p_info.parasite_type_id);
      LOG(INFO) << "Introducing genotype " << p_info.parasite_type_id << " with prevalence: " << p_info.prevalence
                << " : " << num_of_infections << " infections at location " << p_info.location;
      // std::cout << p_info.location << "-" << p_info.parasite_type_id << "-" << num_of_infections << std::endl;
      introduce_parasite(p_info.location, genotype, num_of_infections);
    }
    // update current foi
    update_current_foi();

    // update force of infection for N days
    for (auto d = 0; d < Model::CONFIG->number_of_tracking_days(); d++) {
      for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        force_of_infection_for_N_days_by_location[d][loc] = current_force_of_infection_by_location[loc];
      }
      Model::MOSQUITO->infect_new_cohort_in_PRMC(Model::CONFIG, Model::RANDOM, this, d);
    }
  }
}

void Population::introduce_parasite(const int& location, Genotype* parasite_type, const int& num_of_infections) {
  if (model_ != nullptr) {
    auto persons_bitten_today = Model::RANDOM->roulette_sampling<Person>(
        num_of_infections, Model::POPULATION->individual_relative_biting_by_location[location],
        Model::POPULATION->all_alive_persons_by_location[location], false);

    for (auto* person : persons_bitten_today) {
      initial_infection(person, parasite_type);
    }
  }
}

void Population::initial_infection(Person* person, Genotype* parasite_type) const {
  person->immune_system()->set_increase(true);
  person->set_host_state(Person::ASYMPTOMATIC);

  auto* blood_parasite = person->add_new_parasite_to_blood(parasite_type);

  const auto size =
      model_->random()->random_flat(Model::CONFIG->parasite_density_level().log_parasite_density_from_liver,
                                    Model::CONFIG->parasite_density_level().log_parasite_density_clinical);

  blood_parasite->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
  blood_parasite->set_last_update_log10_parasite_density(size);

  const auto p_clinical = person->get_probability_progress_to_clinical();
  const auto p = model_->random()->random_flat(0.0, 1.0);

  if (p < p_clinical) {
    // progress to clinical after several days
    blood_parasite->set_update_function(model_->progress_to_clinical_update_function());
    person->schedule_progress_to_clinical_event_by(blood_parasite);
  } else {
    // only progress to clearance by Immune system
    // progress to clearance
    blood_parasite->set_update_function(model_->immunity_clearance_update_function());
  }
}

void Population::perform_birth_event() {
  //    std::cout << "Birth Event" << std::endl;

  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    auto poisson_means = size(loc) * Model::CONFIG->birth_rate() / Constants::DAYS_IN_YEAR();
    const auto number_of_births = Model::RANDOM->random_poisson(poisson_means);
    for (auto i = 0; i < number_of_births; i++) {
      give_1_birth(loc);
      Model::DATA_COLLECTOR->update_person_days_by_years(
          loc, Constants::DAYS_IN_YEAR() - Model::SCHEDULER->current_day_in_year());
    }
  }

  //    std::cout << "End Birth Event" << std::endl;
}

void Population::give_1_birth(const int& location) {
  auto p = new Person();
  p->init();
  p->set_age(0);
  p->set_host_state(Person::SUSCEPTIBLE);
  p->set_age_class(0);
  p->set_location(location);
  p->set_residence_location(location);
  p->immune_system()->set_immune_component(new InfantImmuneComponent());
  p->immune_system()->set_latest_immune_value(1.0);
  p->immune_system()->set_increase(false);

  p->set_latest_update_time(Model::SCHEDULER->current_time());
  //                    p->draw_random_immune();

  // set_relative_biting_rate
  p->innate_relative_biting_rate = Model::RANDOM->random_gamma(Model::CONFIG->relative_bitting_info().gamma_a,
                                                               Model::CONFIG->relative_bitting_info().gamma_b);
  p->update_relative_bitting_rate();

  p->set_moving_level(Model::CONFIG->moving_level_generator().draw_random_level(Model::RANDOM));

  p->set_birthday(Model::SCHEDULER->current_time());
  const auto number_of_days_to_next_birthday =
      TimeHelpers::number_of_days_to_next_year(Model::SCHEDULER->calendar_date);
  BirthdayEvent::schedule_event(Model::SCHEDULER, p,
                                Model::SCHEDULER->current_time() + number_of_days_to_next_birthday);

  // schedule for switch
  SwitchImmuneComponentEvent::schedule_for_switch_immune_component_event(
      Model::SCHEDULER, p, Model::SCHEDULER->current_time() + Constants::DAYS_IN_YEAR() / 2);

  //    p->startLivingTime = (Global::startTreatmentDay > Global::scheduler->currentTime) ? Global::startTreatmentDay :
  //    Global::scheduler->currentTime;
  p->generate_prob_present_at_mda_by_age();

  add_person(p);
}

void Population::perform_death_event() {
  //    std::cout << "Death Event" << std::endl;
  // simply change state to dead and release later
  auto pi = get_person_index<PersonIndexByLocationStateAgeClass>();
  if (pi == nullptr) return;

  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      if (hs == Person::DEAD) continue;
      for (auto ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        const int size = pi->vPerson()[loc][hs][ac].size();
        if (size == 0) continue;
        auto poisson_means = size * Model::CONFIG->death_rate_by_age_class()[ac] / Constants::DAYS_IN_YEAR();

        assert(Model::CONFIG->death_rate_by_age_class().size() == Model::CONFIG->number_of_age_classes());
        const auto number_of_deaths = Model::RANDOM->random_poisson(poisson_means);
        if (number_of_deaths == 0) continue;

        //                std::cout << numberOfDeaths << std::endl;
        for (int i = 0; i < number_of_deaths; i++) {
          // change state to Death;
          const int index = Model::RANDOM->random_uniform(size);
          //                    std::cout << index << "-" << pi->vPerson()[loc][hs][ac].size() << std::endl;
          auto* p = pi->vPerson()[loc][hs][ac][index];
          p->cancel_all_events_except(nullptr);
          p->set_host_state(Person::DEAD);
        }
      }
    }
  }
  clear_all_dead_state_individual();
}

void Population::clear_all_dead_state_individual() {
  // return all Death to object pool and clear vPersonIndex[l][dead][ac] for all location and ac
  auto pi = get_person_index<PersonIndexByLocationStateAgeClass>();
  PersonPtrVector removePersons;

  for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      for (auto person : pi->vPerson()[loc][Person::DEAD][ac]) {
        removePersons.push_back(person);
      }
    }
  }

  for (Person* p : removePersons) {
    remove_dead_person(p);
  }
}

void Population::perform_circulation_event() {
  // for each location
  //  get number of circulations based on size * circulation_percent
  //  distributes that number into others location based of other location size
  //  for each number in that list select an individual, and schedule a movement event on next day
  PersonPtrVector today_circulations;

  std::vector<int> v_number_of_residents_by_location(Model::CONFIG->number_of_locations(), 0);

  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    //        v_number_of_residents_by_location[target_location] = (size(target_location));
    v_number_of_residents_by_location[location] = Model::DATA_COLLECTOR->popsize_residence_by_location()[location];
    //        std::cout << v_original_pop_size_by_location[target_location] << std::endl;
  }

  for (int from_location = 0; from_location < Model::CONFIG->number_of_locations(); from_location++) {
    auto poisson_means = size(from_location) * Model::CONFIG->circulation_info().circulation_percent;
    if (poisson_means == 0) continue;
    const auto number_of_circulating_from_this_location = Model::RANDOM->random_poisson(poisson_means);
    if (number_of_circulating_from_this_location == 0) continue;

    DoubleVector v_relative_outmovement_to_destination(Model::CONFIG->number_of_locations(), 0);
    v_relative_outmovement_to_destination = Model::CONFIG->spatial_model()->get_v_relative_out_movement_to_destination(
        from_location, Model::CONFIG->number_of_locations(), Model::CONFIG->spatial_distance_matrix()[from_location],
        v_number_of_residents_by_location);

    std::vector<unsigned int> v_num_leavers_to_destination(
        static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()));

    Model::RANDOM->random_multinomial(static_cast<int>(v_relative_outmovement_to_destination.size()),
                                      static_cast<unsigned int>(number_of_circulating_from_this_location),
                                      &v_relative_outmovement_to_destination[0], &v_num_leavers_to_destination[0]);

    for (int target_location = 0; target_location < Model::CONFIG->number_of_locations(); target_location++) {
      //            std::cout << v_num_leavers_to_destination[target_location] << std::endl;
      if (v_num_leavers_to_destination[target_location] == 0) continue;
      //            std::cout << Model::SCHEDULER->current_time() << "\t" << from_location << "\t" << target_location <<
      //            "\t"
      //                      << v_num_leavers_to_destination[target_location] << std::endl;
      perform_circulation_for_1_location(from_location, target_location, v_num_leavers_to_destination[target_location],
                                         today_circulations);
    }
  }

  for (auto* p : today_circulations) {
    p->randomly_choose_target_location();
  }

  today_circulations.clear();
}

void Population::perform_circulation_for_1_location(const int& from_location, const int& target_location,
                                                    const int& number_of_circulations,
                                                    std::vector<Person*>& today_circulations) {
  auto persons_moving_today = Model::RANDOM->roulette_sampling<Person>(
      number_of_circulations, individual_relative_moving_by_location[from_location],
      all_alive_persons_by_location[from_location], false, sum_relative_moving_by_location[from_location]);

  for (auto* person : persons_moving_today) {
    assert(person->host_state() != Person::DEAD);

    person->today_target_locations()->push_back(target_location);
    today_circulations.push_back(person);
  }
}

bool Population::has_0_case() {
  auto pi = get_person_index<PersonIndexByLocationStateAgeClass>();
  for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    for (int hs = Person::EXPOSED; hs <= Person::CLINICAL; hs++) {
      for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        if (!pi->vPerson()[loc][hs][ac].empty()) {
          return false;
        }
      }
    }
  }
  return true;
}

void Population::initialize_person_indices() {
  const int number_of_location = Model::CONFIG->number_of_locations();
  const int number_of_hoststate = Person::NUMBER_OF_STATE;
  const int number_of_ageclasses = Model::CONFIG->number_of_age_classes();

  auto p_index_by_l_s_a =
      new PersonIndexByLocationStateAgeClass(number_of_location, number_of_hoststate, number_of_ageclasses);
  person_index_list_->push_back(p_index_by_l_s_a);

  auto p_index_location_moving_level = new PersonIndexByLocationMovingLevel(
      number_of_location, Model::CONFIG->circulation_info().number_of_moving_levels);
  person_index_list_->push_back(p_index_location_moving_level);
}

void Population::update_all_individuals() {
  // update all individuals
  auto pi = get_person_index<PersonIndexByLocationStateAgeClass>();
  for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    for (int hs = 0; hs < Person::DEAD; hs++) {
      for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        for (auto* person : pi->vPerson()[loc][hs][ac]) {
          person->update();
        }
      }
    }
  }
}

void Population::persist_current_force_of_infection_to_use_N_days_later() {
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    force_of_infection_for_N_days_by_location[Model::SCHEDULER->current_time()
                                              % Model::CONFIG->number_of_tracking_days()][loc] =
        current_force_of_infection_by_location[loc];
  }
}

void Population::update_current_foi() {
  auto pi = get_person_index<PersonIndexByLocationStateAgeClass>();
  for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    // reset force of infection for each location
    current_force_of_infection_by_location[loc] = 0.0;
    sum_relative_biting_by_location[loc] = 0.0;
    sum_relative_moving_by_location[loc] = 0.0;

    // using clear so as system will not reallocate memory slot for vector
    individual_foi_by_location[loc].clear();
    individual_relative_biting_by_location[loc].clear();
    individual_relative_moving_by_location[loc].clear();
    all_alive_persons_by_location[loc].clear();

    for (int hs = 0; hs < Person::DEAD; hs++) {
      for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        for (auto* person : pi->vPerson()[loc][hs][ac]) {
          double log_10_total_density = person->all_clonal_parasite_populations()->log10_total_infectious_denstiy;

          auto individual_foi =
              person->current_relative_biting_rate * Person::relative_infectivity(log_10_total_density);

          individual_foi_by_location[loc].push_back(individual_foi);
          individual_relative_biting_by_location[loc].push_back(person->current_relative_biting_rate);
          individual_relative_moving_by_location[loc].push_back(
              Model::CONFIG->circulation_info().v_moving_level_value[person->moving_level()]);

          sum_relative_biting_by_location[loc] += person->current_relative_biting_rate;
          sum_relative_moving_by_location[loc] +=
              Model::CONFIG->circulation_info().v_moving_level_value[person->moving_level()];
          current_force_of_infection_by_location[loc] += individual_foi;
          all_alive_persons_by_location[loc].push_back(person);
        }
      }
    }
  }
}
