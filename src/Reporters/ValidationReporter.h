//
// Created by kient on 7/7/2022.
//

#ifndef POMS_VALIDATIONREPORTER_H
#define POMS_VALIDATIONREPORTER_H

#include <fstream>
#include <sstream>
#include "Reporter.h"

class ValidationReporter : public Reporter {
    DISALLOW_COPY_AND_ASSIGN(ValidationReporter)

    DISALLOW_MOVE(ValidationReporter)

public:
    const std::string group_sep = "-1111\t";
    const std::string sep = "\t";
    std::ofstream monthly_data_file;
    std::ofstream summary_data_file;
    std::ofstream gene_db_file;
    std::ofstream gene_freq_file;
    std::ofstream prmc_db_file;
    std::ofstream prmc_freq_file;

public:
    ValidationReporter();

    ~ValidationReporter() override;

    void initialize() override;

    void before_run() override;

    void after_run() override;

    void begin_time_step() override;

    void monthly_report() override;

    void print_EIR_PfPR_by_location(std::stringstream& ss);

    //  void print_monthly_incidence_by_location();
};


#endif //POMS_VALIDATIONREPORTER_H
