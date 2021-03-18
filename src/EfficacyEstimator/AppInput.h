#include <string>
#include <vector>

struct AppInput {
  int population_size{10000};
  int number_of_drugs_in_combination{0};
  std::string input_file{"input.yml"};
  std::vector<int> dosing_days;
  std::vector<double> half_life;
  std::vector<double> k_max;
  std::vector<double> EC50;
  std::vector<double> slope;
};