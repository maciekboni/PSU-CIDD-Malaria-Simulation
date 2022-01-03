//
// Created by pua66 on 3/10/2021.
//

#include "Core/Config/Config.h"
#include "Spatial/WesolowskiSM.h"
#include "Spatial/BarabasiSM.h"
#include "Therapies/Therapy.h"
#include "Therapies/SCTherapy.h"
#include "Strategies/IStrategy.h"
#include "Events/Event.h"
#include "date/date.h"
#include "Events/Population/SingleRoundMDAEvent.h"
#include "gtest/gtest.h"

TEST(ConfigTest, ReadFromDefaultInputFile){
    Config c;
    c.read_from_file("input.yml");

    ASSERT_EQ(c.starting_date(), date::year_month_day{date::year{1990} / 1 / 1});
    ASSERT_EQ(c.ending_date(), date::year_month_day{date::year{1991} / 1 / 1});

    ASSERT_EQ(c.total_time(), 365);

    ASSERT_EQ(c.start_collect_data_day(), 365);
    ASSERT_EQ(c.start_of_comparison_period(), 10926);

    ASSERT_EQ(c.number_of_tracking_days(), 11);
    ASSERT_EQ(c.p_infection_from_an_infectious_bite(), 0.1);

    //ASSERT_EQ(c.age_structure(), std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 15, 20, 60, 100});
    //ASSERT_EQ(c.initial_age_structure(),
    //        std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 25, 35, 45, 55, 65, 100});

    ASSERT_EQ(c.tf_testing_day(), 28);

    ASSERT_EQ(c.days_to_clinical_under_five(), 4);
    ASSERT_EQ(c.days_to_clinical_over_five(), 6);
    ASSERT_EQ(c.days_mature_gametocyte_under_five(), 4);
    ASSERT_EQ(c.days_mature_gametocyte_over_five(), 6);

    ASSERT_EQ(c.p_compliance(), 1.0);
    ASSERT_EQ(c.min_dosing_days(), 1);

    ASSERT_EQ(c.gametocyte_level_full(), 1.0);

    ASSERT_EQ(c.p_relapse(), 0.01);
    ASSERT_EQ(c.relapse_duration(), 30);

    ASSERT_EQ(c.allow_new_coinfection_to_cause_symtoms(), true);
    ASSERT_EQ(c.update_frequency(), 7);
    ASSERT_EQ(c.report_frequency(), 30);

    ASSERT_EQ(c.tf_rate(), 0.1);
    ASSERT_EQ(c.using_free_recombination(), true);
    ASSERT_EQ(c.tf_window_size(), 60);

    ASSERT_EQ(c.using_age_dependent_bitting_level(), false);
    ASSERT_EQ(c.using_variable_probability_infectious_bites_cause_infection(), false);

    ASSERT_EQ(c.fraction_mosquitoes_interrupted_feeding(), 0.0);
    ASSERT_EQ(c.inflation_factor(), 0.01);

    ASSERT_EQ(c.location_db().size(), 9);
    ASSERT_EQ(c.location_db()[0].id, 0);
    ASSERT_EQ(c.location_db()[8].id, 8);
    //ASSERT_EQ(c.location_db()[0].age_distribution,
    //        std::vector<double>{0.0334, 0.0300, 0.0329, 0.0324, 0.0332, 0.0314, 0.0316, 0.0310,
    //                            0.0285, 0.0256, 0.0298, 0.0212, 0.0321, 0.0228, 0.0230, 0.1906, 0.1403, 0.0966,
    //                            0.0605,
    //                            0.0344, 0.0387});
   // ASSERT_EQ(c.location_db()[8].age_distribution,
    //        std::vector<double>{0.0338, 0.0300, 0.0329, 0.0324, 0.0332, 0.0314, 0.0316, 0.0310,
    //                            0.0285, 0.0256, 0.0298, 0.0212, 0.0321, 0.0228, 0.0230, 0.1906, 0.1403, 0.0966,
    //                            0.0605,
    //                            0.0344, 0.0387});

    ASSERT_EQ(c.location_db()[0].p_treatment_less_than_5, 0.6f);
    ASSERT_EQ(c.location_db()[0].p_treatment_more_than_5, 0.5f);

    ASSERT_EQ(c.location_db()[8].p_treatment_less_than_5, 0.6f);
    ASSERT_EQ(c.location_db()[8].p_treatment_more_than_5, 0.5f);

    ASSERT_EQ(c.location_db()[0].beta, 0.05f);
    ASSERT_EQ(c.location_db()[8].beta, 0.05f);

    ASSERT_EQ(c.location_db()[0].population_size, 1000);
    ASSERT_EQ(c.location_db()[8].population_size, 10090);

    ASSERT_EQ(c.birth_rate(), 0.03725);

   // ASSERT_EQ(c.death_rate_by_age_class(),
    //        DoubleVector{0.053979329, 0.018935757, 0.006867257, 0.001124347, 0.001136455, 0.001606066,
    //                     0.001953783, 0.001530096, 0.001299153, 0.001068073, 0.000978264, 0.000978264, 0.0055,
    //                     0.0055,
     //                    0.04450});

 //   ASSERT_EQ(c.mortality_when_treatment_fail_by_age_class(),
  //          DoubleVector{0.040, 0.020, 0.020, 0.020, 0.020, 0.004, 0.004, 0.004, 0.004, 0.004,
   //                      0.004, 0.001, 0.001, 0.001, 0.001});

    ASSERT_EQ(c.parasite_density_level().log_parasite_density_cured, -4.699);
    ASSERT_EQ(c.parasite_density_level().log_parasite_density_from_liver, -2.0);
    ASSERT_EQ(c.parasite_density_level().log_parasite_density_asymptomatic, 3);
    ASSERT_EQ(c.parasite_density_level().log_parasite_density_clinical, 4.301);
    ASSERT_EQ(c.parasite_density_level().log_parasite_density_clinical_from, 3.301);
    ASSERT_EQ(c.parasite_density_level().log_parasite_density_clinical_to, 5.301);
    ASSERT_EQ(c.parasite_density_level().log_parasite_density_detectable, 1.000);
    ASSERT_EQ(c.parasite_density_level().log_parasite_density_pyrogenic, 3.398);

    ASSERT_EQ(c.genotype_info().loci_vector.size(), 5);

    ASSERT_EQ(c.relative_infectivity().sigma, log(10) / 3.91);

    ASSERT_EQ(c.number_of_age_classes(), 15);

    ASSERT_EQ(c.number_of_locations(), 9);

    ASSERT_EQ(c.spatial_distance_matrix().size(), 9);
    ASSERT_EQ(c.spatial_distance_matrix()[8].size(), 9);

    ASSERT_EQ(c.seasonal_info().A, DoubleVector(9, 1.0));

    ASSERT_NE(dynamic_cast<Spatial::WesolowskiSM*>(c.spatial_model()), nullptr);
    ASSERT_EQ(dynamic_cast<Spatial::WesolowskiSM*>(c.spatial_model())->kappa(), 0.01093251);
    ASSERT_EQ(dynamic_cast<Spatial::BarabasiSM*>(c.spatial_model()), nullptr);

    ASSERT_EQ(c.immune_system_information().acquire_rate, 0.00125);
    ASSERT_EQ(c.immune_system_information().decay_rate, 0.0025);


    ASSERT_EQ(c.drug_db()->size(), 7);
    ASSERT_EQ(c.drug_db()->at(0)->n(), 25);
    ASSERT_EQ(c.drug_db()->at(6)->n(), 19);

    //
  // TODO: rework on this

//    ASSERT_EQ(c.EC50_power_n_table().size(), 128);
//    ASSERT_EQ(c.EC50_power_n_table()[0].size(), 7);

    ASSERT_EQ(c.circulation_info().max_relative_moving_value, 35);
    //
    ASSERT_EQ(c.relative_bitting_info().max_relative_biting_value, 35);

    ASSERT_EQ(c.therapy_db().size(), 13);
    ASSERT_EQ(c.therapy_db()[0]->drug_ids.size(), 1);
    ASSERT_EQ(c.therapy_db()[0]->drug_ids, IntVector{0});
    ASSERT_EQ(((SCTherapy*) c.therapy_db()[0])->dosing_day[0], 3);

    ASSERT_EQ(c.therapy_db()[11]->drug_ids.size(), 3);
    //ASSERT_EQ(c.therapy_db()[11]->drug_ids, IntVector{0, 1, 2});
    ASSERT_EQ(((SCTherapy*) c.therapy_db()[11])->dosing_day[0], 3);

    ASSERT_EQ(c.strategy_db().size(), 15);
    ASSERT_EQ(c.strategy_db()[0]->name, "SP-AQ-CQ-AL-MFTStrategy");
    ASSERT_EQ(c.strategy_db()[0]->get_type(), IStrategy::MFT);
    ASSERT_EQ(c.strategy_db()[14]->name, "BaseLineStrategy");
    ASSERT_EQ(c.strategy_db()[14]->get_type(), IStrategy::NestedMFTMultiLocation);

    ASSERT_EQ(c.initial_strategy_id(), 5);

    // ASSERT_EQ(c.initial_parasite_info()[18].parasite_type_id, 15);
    // ASSERT_EQ(c.initial_parasite_info()[18].prevalence, 0.05);
    // ASSERT_EQ(c.initial_parasite_info()[18].location, 10);
    // ASSERT_EQ(c.initial_parasite_info()[19].parasite_type_id, 20);
    // ASSERT_EQ(c.initial_parasite_info()[19].prevalence, 0.1);
    // ASSERT_EQ(c.initial_parasite_info()[19].location, 10);

    ASSERT_EQ(c.preconfig_population_events().size(), 15);
    ASSERT_EQ(c.preconfig_population_events()[0]->time, 78);
    ASSERT_EQ(c.preconfig_population_events()[1]->time, 78);
    ASSERT_EQ(c.preconfig_population_events()[2]->time, 104);
    ASSERT_EQ(c.preconfig_population_events()[3]->time, 236);
    ASSERT_EQ(c.preconfig_population_events()[4]->time, 292);
    ASSERT_EQ(c.preconfig_population_events()[5]->time, 152);
    ASSERT_EQ(c.preconfig_population_events()[6]->time, 31);
    ASSERT_EQ(c.preconfig_population_events()[7]->time, 59);
    ASSERT_EQ(c.preconfig_population_events()[8]->time, 151);
    ASSERT_EQ(c.preconfig_population_events()[9]->time, 60);
    ASSERT_EQ(c.preconfig_population_events()[10]->time, 152);
    ASSERT_EQ(c.preconfig_population_events()[11]->time, 244);
    ASSERT_EQ(c.preconfig_population_events()[12]->time,10957);

    ASSERT_EQ(dynamic_cast<SingleRoundMDAEvent*>(c.preconfig_population_events()[12])->fraction_population_targeted[0],
            1);
    ASSERT_EQ(dynamic_cast<SingleRoundMDAEvent*>(c.preconfig_population_events()[12])->fraction_population_targeted[1],
            1);
    ASSERT_EQ(dynamic_cast<SingleRoundMDAEvent*>(c.preconfig_population_events()[12])->fraction_population_targeted[2],
            1);
    ASSERT_EQ(dynamic_cast<SingleRoundMDAEvent*>(c.preconfig_population_events()[12])->days_to_complete_all_treatments,
            14);

    ASSERT_EQ(c.preconfig_population_events()[13]->time, 10995);
    ASSERT_EQ(c.preconfig_population_events()[14]->time, 11030);

    ASSERT_EQ(c.bitting_level_generator().level_density.size(), 100);
    ASSERT_EQ(c.moving_level_generator().level_density.size(), 100);


    // ASSERT_EQ(2, 2);

}

