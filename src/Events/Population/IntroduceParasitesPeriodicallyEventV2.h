//
// Created by nguyentd on 11/2/2021.
//

#ifndef POMS_SRC_EVENTS_POPULATION_INTRODUCEPARASITESPERIODICALLYEVENTV2_H
#define POMS_SRC_EVENTS_POPULATION_INTRODUCEPARASITESPERIODICALLYEVENTV2_H


#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Events/Event.h"

class IntroduceParasitesPeriodicallyEventV2  : public Event {
  DISALLOW_COPY_AND_ASSIGN(IntroduceParasitesPeriodicallyEventV2)

  OBJECTPOOL(IntroduceParasitesPeriodicallyEventV2)

  VIRTUAL_PROPERTY_REF(int, location)

  VIRTUAL_PROPERTY_REF(int, duration)

  VIRTUAL_PROPERTY_REF(int, number_of_cases)



public:
  std::vector<std::vector<double>> allele_distributions;
  int start_day;
  int end_day;

public:
  IntroduceParasitesPeriodicallyEventV2( const std::vector<std::vector<double>> & allele_distributions_in = std::vector<std::vector<double>>(),
                                         const int &location = -1, const int &duration = -1,
                                        const int &number_of_cases = -1, const int &start_day_in = -1,
                                         const int & end_day_in = -1);

  //    ImportationEvent(const ImportationEvent& orig);
  virtual ~IntroduceParasitesPeriodicallyEventV2();

  static void schedule_event(Scheduler *scheduler, IntroduceParasitesPeriodicallyEventV2* old_event);

  std::string name() override {
    return "IntroduceParasitesPeriodicallyEventV2";
  }

private:
  void execute() override;

};


#endif //POMS_SRC_EVENTS_POPULATION_INTRODUCEPARASITESPERIODICALLYEVENTV2_H
