#ifndef POMS_BFREPORTER_H
#define POMS_BFREPORTER_H

#include "Reporters/Reporter.h"
#include <sstream>
#include <fstream>

class AppInput;

class PkPdReporter : public Reporter {
  DISALLOW_COPY_AND_ASSIGN(PkPdReporter)

  DISALLOW_MOVE(PkPdReporter)

public:
  std::stringstream ss;
  const std::string group_sep = "-1111\t";
  const std::string sep = ",";

  PkPdReporter(AppInput* appInput=nullptr);

  ~PkPdReporter() override;

  void initialize() override;

  void before_run() override;

  void after_run() override;

  void begin_time_step() override;

  void monthly_report() override;

private:
  AppInput* appInput{nullptr};
  std::ofstream outputFStream;
  //  void print_monthly_incidence_by_location();
};

#endif
