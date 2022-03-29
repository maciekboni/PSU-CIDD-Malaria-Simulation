//
// Created by kient on 3/28/2022.
//

#include "ChangeInterruptedFeedingRateEvent.h"

ChangeInterruptedFeedingRateEvent::ChangeInterruptedFeedingRateEvent(const int &location, const double &ifr, const int &at_time)
    : location{location},
      ifr{ifr} {
  time = at_time;
}

void ChangeInterruptedFeedingRateEvent::execute() {
  Model::CONFIG->mosquito_config().interrupted_feeding_rate[location] = ifr;
  LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " : Change interrupted feeding rate at location " << location << " to " << ifr;
}