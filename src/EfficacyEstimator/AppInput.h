#include <string>
#include <vector>

struct AppInput {
  int population_size { 10000 };
  int number_of_drugs_in_combination { 0 };
  std::string input_file { "input.yml" };
  bool is_art { false };
  bool to_file { false };
  bool to_console { false };
  std::string output_file { "" };
  std::vector<int> dosing_days;
  std::vector<double> half_life;
  std::vector<double> k_max;
  std::vector<double> EC50;
  std::vector<double> slope;
  std::vector<double> mean_drug_absorption;
};
