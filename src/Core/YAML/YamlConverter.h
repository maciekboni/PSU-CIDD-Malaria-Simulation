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
struct convert<GenotypeInfo> {
  static Node encode(const GenotypeInfo& rhs) {
    Node node;
    node.push_back("GenotypeInfo");
    return node;
  }

  static bool decode(const Node& node, GenotypeInfo& genotype_info) {
    genotype_info.loci_vector.clear();
    for (std::size_t i = 0; i < node["loci"].size(); i++) {
      Locus l;
      l.position = node["loci"][i]["position"].as<int>();

      for (std::size_t j = 0; j < node["loci"][i]["alleles"].size(); j++) {
        Allele al;
        al.value = node["loci"][i]["alleles"][j]["value"].as<int>();
        al.name = node["loci"][i]["alleles"][j]["allele_name"].as<std::string>();
        al.short_name = node["loci"][i]["alleles"][j]["short_name"].as<std::string>();
        al.mutation_level = node["loci"][i]["alleles"][j]["mutation_level"].as<int>();
        al.daily_cost_of_resistance = node["loci"][i]["alleles"][j]["daily_cost_of_resistance"].as<double>();
        for (std::size_t c = 0; c < node["loci"][i]["alleles"][j]["can_mutate_to"].size(); c++) {
          //                al.mutation_value_up.push_back(node["loci"][i]["alleles"][j]["mutation_up"][c].as<int>());
          al.mutation_values.push_back(node["loci"][i]["alleles"][j]["can_mutate_to"][c].as<int>());
        }

        l.alleles.push_back(al);
      }

      genotype_info.loci_vector.push_back(l);
    }
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
struct convert<GeneInfo> {
  static Node encode(const GeneInfo& rhs) {
    Node node;
    node.push_back("GeneInfo");
    return node;
  }

  static bool decode(const Node& node, GeneInfo& gene_info) {
    gene_info.genes.clear();

    for (const auto& gene_node : node) {
      auto gene = gene_node.as<Gene>();
      gene_info.genes.push_back(gene);
    }
    return true;
  }
};

template <>
struct convert<Gene> {
  static Node encode(const Gene& rhs) {
    Node node;
    node.push_back("Gene");
    return node;
  }

  static bool decode(const Node& node, Gene& gene) {
    gene.aa_positions.clear();
    gene.name = node["name"].as<std::string>();
    gene.chromosome = node["chromosome"].as<int>();

    gene.max_copy = node["max_copy"] ? node["max_copy"].as<int>() : 1;
    gene.copy_daily_crs =
        node["copy_daily_crs"] ? node["copy_daily_crs"].as<std::vector<double>>() : std::vector<double>();

    for (const auto& aa_node : node["aa_positions"]) {
      auto aa_position = aa_node.as<AaPosition>();
      gene.aa_positions.push_back(aa_position);
    }
    return true;
  }
};

template <>
struct convert<AaPosition> {
  static Node encode(const AaPosition& rhs) {
    Node node;
    node.push_back("AaPosition");
    return node;
  }

  static bool decode(const Node& node, AaPosition& aa_pos) {
    aa_pos.amino_acids.clear();
    aa_pos.daily_crs.clear();
    aa_pos.position = node["position"].as<int>();
    aa_pos.amino_acids = node["amino_acids"].as<std::vector<char>>();
    aa_pos.daily_crs = node["daily_crs"].as<std::vector<double>>();

    return true;
  }
};

}  // namespace YAML
#endif  // YAMLCONVERTER_H
