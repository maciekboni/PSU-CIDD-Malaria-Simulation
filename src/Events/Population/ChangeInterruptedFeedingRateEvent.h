//
// Created by kient on 3/28/2022.
//

#ifndef POMS_CHANGEINTERRUPTEDFEEDINGRATEEVENT_H
#define POMS_CHANGEINTERRUPTEDFEEDINGRATEEVENT_H

#include "Core/PropertyMacro.h"
#include "Events/Event.h"
#include "Core/Scheduler.h"
#include <string>
#include <vector>
#include <easylogging++.h>
#include "Model.h"
#include "Core/Config/Config.h"

class ChangeInterruptedFeedingRateEvent : public Event {
  DISALLOW_COPY_AND_ASSIGN(ChangeInterruptedFeedingRateEvent)
  DISALLOW_MOVE(ChangeInterruptedFeedingRateEvent)
public:
  int location {-1};
  double ifr {0.0};
public:
  explicit ChangeInterruptedFeedingRateEvent(const int &location = -1, const double &ifr = 0.0, const int &at_time = -1);

  ~ChangeInterruptedFeedingRateEvent() override = default;

  std::string name() override {
    return "ChangeInterruptedFeedingRateEvent";
  }

private:
  void execute() override;
};

#endif  // POMS_CHANGEINTERRUPTEDFEEDINGRATEEVENT_H
