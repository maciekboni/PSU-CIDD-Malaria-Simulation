#include "NovelDrugSwitchingStrategy.h"
#include "Model.h"
#include "Core/Random.h"
#include "Therapies/Therapy.h"
#include "MDC/ModelDataCollector.h"
#include "Core/Config/Config.h"
#include "Helpers/TimeHelpers.h"
#include "Core/Scheduler.h"
#include "SFTStrategy.h"
#include "MFTStrategy.h"

NovelDrugSwitchingStrategy::NovelDrugSwitchingStrategy() {
  name = "NovelDrugSwitchingStrategy";
  type = NovelDrugSwitching;
}


std::string NovelDrugSwitchingStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << "-";
  sstm << NestedMFTStrategy::to_string();
  return sstm.str();
}

void NovelDrugSwitchingStrategy::monthly_update() {
  NestedMFTStrategy::monthly_update();

  auto public_sector_strategy = strategy_list[0];

  int current_public_therapy_id =
      public_sector_strategy->type == SFT
      ? dynamic_cast<SFTStrategy*>(public_sector_strategy)->therapy_list()[0]->id()
      : dynamic_cast<MFTStrategy*>(public_sector_strategy)->therapy_list[0]->id();

  if (!is_switched) {
    if (Model::DATA_COLLECTOR->current_tf_by_therapy()[current_public_therapy_id] >= tf_threshold) {

      // switch to novel drugs

      auto novel_SFT_strategy = Model::CONFIG->strategy_db()[switch_to];

      auto* new_public_stategy = new NestedMFTStrategy();

      new_public_stategy->strategy_list.push_back(public_sector_strategy);
      new_public_stategy->strategy_list.push_back(novel_SFT_strategy);
      new_public_stategy->distribution.push_back(1 - replace_fraction);
      new_public_stategy->distribution.push_back(replace_fraction);

      new_public_stategy->start_distribution.push_back(1);
      new_public_stategy->start_distribution.push_back(0);

      new_public_stategy->peak_distribution.push_back(1 - replace_fraction);
      new_public_stategy->peak_distribution.push_back(replace_fraction);

      new_public_stategy->peak_after = replace_duration;
      new_public_stategy->starting_time = Model::SCHEDULER->current_time();

      strategy_list[0] = new_public_stategy;

      Model::CONFIG->strategy_db().push_back(new_public_stategy);

      //reset the time point to collect ntf
      Model::CONFIG->start_of_comparison_period() = Model::SCHEDULER->current_time();

      //reset the total time to 10 years after this time point
      const date::sys_days next_10_year{date::year_month_day{Model::SCHEDULER->calendar_date} + date::years{10}};
      const auto new_total_time = Model::SCHEDULER->current_time() + TimeHelpers::number_of_days(
          Model::SCHEDULER->calendar_date, next_10_year
      );

      if (new_total_time > Model::CONFIG->total_time()) {
        //extend the scheduler
        Model::SCHEDULER->extend_total_time(new_total_time);
      }
      Model::CONFIG->total_time() = new_total_time;

      LOG(INFO) << date::year_month_day{Model::SCHEDULER->calendar_date} << ": Switch to novel drug with id "
                << switch_to;

      is_switched = true;
    } else {

      // check and extend total time if total time is less than 10 years
      const date::sys_days next_10_year{date::year_month_day{Model::SCHEDULER->calendar_date} + date::years{10}};
      const auto new_total_time = Model::SCHEDULER->current_time() + TimeHelpers::number_of_days(
          Model::SCHEDULER->calendar_date, next_10_year
      );

      if (new_total_time > Model::CONFIG->total_time()) {
        //extend the scheduler
        Model::SCHEDULER->extend_total_time(new_total_time + 10 * 365);
        Model::CONFIG->total_time() = new_total_time + 10 * 365;
        Model::CONFIG->start_of_comparison_period() = new_total_time + 10 * 365;
      }
    }
  }
}
