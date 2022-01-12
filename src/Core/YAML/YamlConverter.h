#ifndef YAMLCONVERTER_H
#define YAMLCONVERTER_H

#include <date/date.h>
#include <yaml-cpp/yaml.h>

#include <cmath>

#include "Core/TypeDef.h"
#include "Spatial/Location.h"

namespace YAML {
template <>
struct convert<date::sys_days> {
  static Node encode(const date::sys_days& rhs) {
    Node node;
    node.push_back(date::format("%Y/%m/%d", rhs));
    return node;
  }

  static bool decode(const Node& node, date::sys_days& rhs) {
    if (!node.IsScalar()) {
      return false;
    }
    std::stringstream ss(node.as<std::string>());
    date::from_stream(ss, "%Y/%m/%d", rhs);
    return true;
  }
};

template <>
struct convert<date::year_month_day> {
  static Node encode(const date::year_month_day& rhs) {
    Node node;
    node.push_back(date::format("%Y/%m/%d", rhs));
    return node;
  }

  static bool decode(const Node& node, date::year_month_day& rhs) {
    if (!node.IsScalar()) {
      return false;
    }
    std::stringstream ss(node.as<std::string>());
    from_stream(ss, "%Y/%m/%d", rhs);
    return true;
  }
};

template <>
struct convert<std::vector<Spatial::Location>> {
  static Node encode(const std::vector<Spatial::Location>& rhs) {
    Node node;
    node.push_back("location_db");
    return node;
  }

  static bool decode(const Node& node, std::vector<Spatial::Location>& location_db) {
    //
    // if (!node.IsScalar()) {
    //   return false;
    // }
    const auto number_of_locations = node["location_info"].size();
    for (std::size_t i = 0; i < number_of_locations; i++) {
      location_db.emplace_back(node["location_info"][i][0].as<int>(), node["location_info"][i][1].as<float>(),
                               node["location_info"][i][2].as<float>(), 0);
    }
    for (std::size_t loc = 0; loc < number_of_locations; loc++) {
      auto input_loc = node["age_distribution_by_location"].size() < number_of_locations ? 0 : loc;

      for (std::size_t i = 0; i < node["age_distribution_by_location"][input_loc].size(); i++) {
        location_db[loc].age_distribution.push_back(node["age_distribution_by_location"][input_loc][i].as<double>());
      }
    }
    for (std::size_t loc = 0; loc < number_of_locations; loc++) {
      auto input_loc = node["p_treatment_for_less_than_5_by_location"].size() < number_of_locations ? 0 : loc;
      location_db[loc].p_treatment_less_than_5 = node["p_treatment_for_less_than_5_by_location"][input_loc].as<float>();
    }
    for (std::size_t loc = 0; loc < number_of_locations; loc++) {
      auto input_loc = node["p_treatment_for_more_than_5_by_location"].size() < number_of_locations ? 0 : loc;
      location_db[loc].p_treatment_more_than_5 = node["p_treatment_for_more_than_5_by_location"][input_loc].as<float>();
    }
    for (std::size_t loc = 0; loc < number_of_locations; loc++) {
      auto input_loc = node["beta_by_location"].size() < number_of_locations ? 0 : loc;
      location_db[loc].beta = node["beta_by_location"][input_loc].as<float>();
    }
    for (std::size_t loc = 0; loc < number_of_locations; loc++) {
      auto input_loc = node["population_size_by_location"].size() < number_of_locations ? 0 : loc;
      location_db[loc].population_size = node["population_size_by_location"][input_loc].as<int>();
    }
    return true;
  }
};

template <>
struct convert<ParasiteDensityLevel> {
  static Node encode(const ParasiteDensityLevel& rhs) {
    Node node;
    node.push_back("ParasiteDensityLevel");
    return node;
  }

  static bool decode(const Node& node, ParasiteDensityLevel& parasite_density_level) {
    //
    // if (!node.IsScalar()) {
    //   return false;
    parasite_density_level.log_parasite_density_cured = node["log_parasite_density_cured"].as<double>();
    parasite_density_level.log_parasite_density_from_liver = node["log_parasite_density_from_liver"].as<double>();
    parasite_density_level.log_parasite_density_asymptomatic = node["log_parasite_density_asymptomatic"].as<double>();
    parasite_density_level.log_parasite_density_clinical = node["log_parasite_density_clinical"].as<double>();
    parasite_density_level.log_parasite_density_clinical_from = node["log_parasite_density_clinical_from"].as<double>();
    parasite_density_level.log_parasite_density_clinical_to = node["log_parasite_density_clinical_to"].as<double>();
    parasite_density_level.log_parasite_density_detectable = node["log_parasite_density_detectable"].as<double>();

    parasite_density_level.log_parasite_density_detectable_pfpr =
        node["log_parasite_density_detectable_pfpr"] ? node["log_parasite_density_detectable_pfpr"].as<double>()
                                                     : node["log_parasite_density_detectable"].as<double>();
    parasite_density_level.log_parasite_density_pyrogenic = node["log_parasite_density_pyrogenic"].as<double>();

    return true;
  }
};

template <>
struct convert<RelativeInfectivity> {
  static Node encode(const RelativeInfectivity& rhs) {
    Node node;
    node.push_back("RelativeInfectivity");
    return node;
  }

  static bool decode(const Node& node, RelativeInfectivity& relative_infectivity) {
    relative_infectivity.sigma = node["sigma"].as<double>();
    const auto ro = node["ro"].as<double>();
    const auto blood_meal_volume = node["blood_meal_volume"].as<double>();

    const auto d_star = 1 / blood_meal_volume;

    relative_infectivity.ro_star = (log(ro) - log(d_star)) / relative_infectivity.sigma;

    relative_infectivity.sigma = log(10) / relative_infectivity.sigma;
    return true;
  }
};

template <>
struct convert<PfGeneInfo> {
  static Node encode(const PfGeneInfo& rhs) {
    Node node;
    node.push_back("GeneInfo");
    return node;
  }

  static bool decode(const Node& node, PfGeneInfo& gene_info) {
    for (const auto& chromosome_node : node) {
      auto chromosome = chromosome_node["chromosome"].as<int>();
      for (const auto& gene_node : chromosome_node["genes"]) {
        auto gene = gene_node.as<GeneInfo>();
        gene.chromosome = chromosome;
        gene_info.chromosome_infos[chromosome - 1].gene_infos.push_back(gene);
      }
    }
    return true;
  }
};

template <>
struct convert<GeneInfo> {
  static Node encode(const GeneInfo& rhs) {
    Node node;
    node.push_back("Gene");
    return node;
  }

  static bool decode(const Node& node, GeneInfo& gene) {
    gene.aa_position_infos.clear();
    gene.name = node["name"].as<std::string>();

    gene.max_copies = node["max_copies"] ? node["max_copies"].as<int>() : 1;
    gene.cnv_daily_crs =
        node["cnv_daily_crs"] ? node["cnv_daily_crs"].as<std::vector<double>>() : std::vector<double>();

    if (node["cnv_multiplicative_effect_on_EC50"]) {
      for (const auto& drug_node : node["cnv_multiplicative_effect_on_EC50"]) {
        gene.cnv_multiplicative_effect_on_EC50[drug_node["drug_id"].as<int>()] =
            drug_node["factors"].as<std::vector<double>>();
      }
    }

    gene.multiplicative_effect_on_EC50_for_2_or_more_mutations =
        node["multiplicative_effect_on_EC50_for_2_or_more_mutations"]
            ? node["multiplicative_effect_on_EC50_for_2_or_more_mutations"].as<double>()
            : 1;

    for (const auto& aa_node : node["aa_positions"]) {
      auto aa_position = aa_node.as<AaPositionInfo>();
      gene.aa_position_infos.push_back(aa_position);
    }
    return true;
  }
};

template <>
struct convert<AaPositionInfo> {
  static Node encode(const AaPositionInfo& rhs) {
    Node node;
    node.push_back("AaPosition");
    return node;
  }

  static bool decode(const Node& node, AaPositionInfo& aa_pos) {
    aa_pos.amino_acids.clear();
    aa_pos.daily_crs.clear();
    aa_pos.position = node["position"].as<int>();
    aa_pos.amino_acids = node["amino_acids"].as<std::vector<char>>();
    aa_pos.daily_crs = node["daily_crs"].as<std::vector<double>>();

    for (const auto& drug_node : node["multiplicative_effect_on_EC50"]) {
      aa_pos.multiplicative_effect_on_EC50[drug_node["drug_id"].as<int>()] =
          drug_node["factors"].as<std::vector<double>>();
    }

    return true;
  }
};

}  // namespace YAML
#endif  // YAMLCONVERTER_H
