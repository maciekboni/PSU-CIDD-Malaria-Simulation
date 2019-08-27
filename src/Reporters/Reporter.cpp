/* 
 * File:   Reporter.cpp
 * Author: Merlin
 * 
 * Created on August 1, 2013, 12:05 PM
 */

#include "Reporter.h"

#include "ConsoleReporter.h"
#include "Constants.h"
#include "Core/Config/Config.h"
#include "easylogging++.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "MonthlyReporter.h"
#include "MMCReporter.h"
#include "SpatialReporter.h"

std::map<std::string, Reporter::ReportType> Reporter::ReportTypeMap{
    {"Console", CONSOLE},
    {"MonthlyReporter", MONTHLY_REPORTER},
    {"MMC", MMC_REPORTER},
    {"SpatialReporter", SPATIAL_REPORTER}
};

// Calculate the number of treatment failures (NTF) for the model
double Reporter::calculate_treatment_failures() {
  const auto total_time_in_years = (Model::SCHEDULER->current_time() - Model::CONFIG->start_of_comparison_period()) /
    static_cast<double>(Constants::DAYS_IN_YEAR());

  auto sum_ntf = 0.0;
  ul pop_size = 0;
  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++)
  {
    sum_ntf += Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location];
    pop_size += Model::DATA_COLLECTOR->popsize_by_location()[location];
  }

  return (sum_ntf * 100 / pop_size) / total_time_in_years;
}

Reporter *Reporter::MakeReport(ReportType report_type) {
  switch (report_type) {
    case CONSOLE:return new ConsoleReporter();
    case MONTHLY_REPORTER:return new MonthlyReporter();
    case MMC_REPORTER:return new MMCReporter();
    case SPATIAL_REPORTER: return new SpatialReporter();
    default:
      LOG(WARNING) << "No reporter type supplied, returning MonthlyReporter";
      return new MonthlyReporter();
  }
}