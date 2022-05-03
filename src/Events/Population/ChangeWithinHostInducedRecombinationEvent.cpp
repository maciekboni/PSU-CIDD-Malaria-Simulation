//
// Created by kient on 5/3/2022.
//

#include "ChangeWithinHostInducedRecombinationEvent.h"
ChangeWithinHostInducedRecombinationEvent::ChangeWithinHostInducedRecombinationEvent(const bool& value,
                                                                                     const int& at_time)
    : value { value } {
  time = at_time;
}
void ChangeWithinHostInducedRecombinationEvent::execute() {
  Model::CONFIG->within_host_induced_free_recombination() = value;
  LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " : Change within host induced recombination to " << value;
}
