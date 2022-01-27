/*
 * File:   TypeDef.h
 * Author: nguyentran
 *
 * Created on April 17, 2013, 10:17 AM
 */

#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <array>
#include <list>
#include <map>
#include <ostream>
#include <string>
#include <vector>

class Person;

class PersonIndex;

class Event;

class Reporter;

class Drug;

class IStrategy;

class Therapy;

typedef unsigned long ul;

typedef std::vector<double> DoubleVector;
typedef std::vector<DoubleVector> DoubleVector2;
typedef std::vector<DoubleVector2> DoubleVector3;
typedef std::vector<int> IntVector;
typedef std::vector<int> *IntVectorPtr;
typedef std::vector<IntVector> IntVector2;
typedef std::vector<IntVector2> IntVector3;
typedef std::vector<IntVector *> IntVectorPtrVector;
typedef std::vector<IntVector> *IntVector2Ptr;
typedef std::vector<unsigned int> UIntVector;

typedef std::vector<ul> LongVector;
typedef std::vector<LongVector> LongVector2;

typedef std::vector<std::string> StringVector;
typedef std::vector<StringVector> StringVector2;

typedef std::map<int, int> IntIntMap;

typedef std::vector<Person *> PersonPtrVector;
typedef PersonPtrVector::iterator PersonPtrVectorIterator;

typedef std::vector<PersonPtrVector> PersonPtrVector2;
typedef std::vector<PersonPtrVector2> PersonPtrVector3;
typedef std::vector<PersonPtrVector3> PersonPtrVector4;

typedef std::vector<Event *> EventPtrVector;
typedef std::vector<EventPtrVector> EventPtrVector2;

typedef std::vector<Reporter *> ReporterPtrVector;

typedef std::list<PersonIndex *> PersonIndexPtrList;

typedef std::map<int, Drug *> DrugPtrMap;

typedef std::vector<Therapy *> TherapyPtrVector;
typedef std::vector<IStrategy *> StrategyPtrVector;

struct SeasonalInfo {
  bool enable { false };
  DoubleVector A;
  DoubleVector B;
  DoubleVector C;
  DoubleVector phi;
  DoubleVector min_value;

  friend std::ostream &operator<<(std::ostream &os, const SeasonalInfo &seasonal_info);
};

inline std::ostream &operator<<(std::ostream &os, const SeasonalInfo &seasonal_info) {
  os << "seasonal_info: ";
  return os;
}

struct ImmuneSystemInformation {
  double acquire_rate { -1 };
  std::vector<double> acquire_rate_by_age;
  double decay_rate { -1 };

  double duration_for_fully_immune { -1 };
  double duration_for_naive { -1 };

  //    double mean_initial_condition;
  //    double sd_initial_condition;

  double immune_inflation_rate { -1 };

  double min_clinical_probability { -1 };
  double max_clinical_probability { -1 };

  double immune_effect_on_progression_to_clinical { -1 };

  double c_min { -1 };
  double c_max { -1 };

  double alpha_immune { -1 };
  double beta_immune { -1 };

  double age_mature_immunity { -1 };
  double factor_effect_age_mature_immunity { -1 };
};

struct ParasiteDensityLevel {
  double log_parasite_density_cured;
  double log_parasite_density_from_liver;
  double log_parasite_density_asymptomatic;
  double log_parasite_density_clinical;
  double log_parasite_density_clinical_from;
  double log_parasite_density_clinical_to;
  double log_parasite_density_detectable;
  double log_parasite_density_detectable_pfpr;
  double log_parasite_density_pyrogenic;

  friend std::ostream &operator<<(std::ostream &os, const ParasiteDensityLevel &pdl) {
    os << "[" << pdl.log_parasite_density_cured << "," << pdl.log_parasite_density_from_liver << ","
       << pdl.log_parasite_density_asymptomatic << "," << pdl.log_parasite_density_clinical << ","
       << pdl.log_parasite_density_clinical_from << "," << pdl.log_parasite_density_clinical_to << ","
       << pdl.log_parasite_density_detectable << "," << pdl.log_parasite_density_detectable_pfpr << ","
       << pdl.log_parasite_density_pyrogenic << "]";
    return os;
  }
};

struct RelativeBittingInformation {
  double max_relative_biting_value;
  int number_of_biting_levels;

  //  biting_level_distribution:
  //  #  distribution: Exponential
  //    distribution: Gamma
  //    Exponential:
  double scale;

  double mean;
  double sd;
  DoubleVector v_biting_level_value;
  DoubleVector v_biting_level_density;
};

struct RelativeMovingInformation {
  double max_relative_moving_value;
  int number_of_moving_levels;

  //  biting_level_distribution:
  //  #  distribution: Exponential
  //    distribution: Gamma
  //    Exponential:
  double scale;

  double mean;
  double sd;
  DoubleVector v_moving_level_value;
  DoubleVector v_moving_level_density;

  double circulation_percent;
  double length_of_stay_mean;
  double length_of_stay_sd;
  double length_of_stay_theta;
  double length_of_stay_k;
};

struct InitialParasiteInfo {
  int location;
  int parasite_type_id;
  double prevalence;

  InitialParasiteInfo() : location(-1), parasite_type_id(-1), prevalence(-1.0) {};

  InitialParasiteInfo(const int loc, const int p_type, const double pre)
      : location(loc), parasite_type_id(p_type), prevalence(pre) {};
};

struct RelativeInfectivity {
  double sigma;
  double ro_star;

  friend std::ostream &operator<<(std::ostream &os, const RelativeInfectivity &e) {
    os << "[" << e.sigma << "," << e.ro_star << "]";
    return os;
  }
};

struct AaPositionInfo {
  int position { -1 };
  std::vector<char> amino_acids;
  std::vector<double> daily_crs;
  std::map<int, std::vector<double>> multiplicative_effect_on_EC50;
  friend std::ostream &operator<<(std::ostream &os, const AaPositionInfo &aa) { return os; }
};

struct GeneInfo {
  std::string name;
  int chromosome { -1 };
  int max_copies { 1 };
  std::vector<double> cnv_daily_crs;
  std::map<int, std::vector<double>> cnv_multiplicative_effect_on_EC50;
  std::vector<AaPositionInfo> aa_position_infos;
  std::map<int, double> multiplicative_effect_on_EC50_for_2_or_more_mutations;
  friend std::ostream &operator<<(std::ostream &os, const GeneInfo &aa) { return os; }
};

struct ChromosomeInfo {
  std::vector<GeneInfo> gene_infos;

  friend std::ostream &operator<<(std::ostream &os, const ChromosomeInfo &chromosome) { return os; }
};
struct PfGeneInfo {
  std::array<ChromosomeInfo, 14> chromosome_infos {};

  friend std::ostream &operator<<(std::ostream &os, const PfGeneInfo &geneInfo) { return os; }
  int calculate_aa_pos(int chromosome_id, int gene_id, int aa_pos_id, int aa_id) {
    auto result = 0;
    for (int ii = 0; ii < chromosome_id; ++ii) {
      result += chromosome_infos[ii].gene_infos.size() > 1 ? chromosome_infos[ii].gene_infos.size() - 1 : 0;  // for ','
      for (auto &gene_info : chromosome_infos[ii].gene_infos) {
        result += gene_info.aa_position_infos.size();
        result += gene_info.max_copies > 1 ? 1 : 0;  // for copy number
      }
    }
    result += chromosome_id;  // for "|"

    // final chromosome
    for (int ii = 0; ii < gene_id; ++ii) {
      result += chromosome_infos[chromosome_id].gene_infos[ii].aa_position_infos.size();
      result += chromosome_infos[chromosome_id].gene_infos[ii].max_copies > 1 ? 1 : 0;  // for copy number
      result += 1;                                                                      // for ","
    }

    // final gene
    result += aa_id;
    return result;
  }
};

struct OverrideEC50Pattern {
  std::string pattern;
  int drug_id;
  double ec50;

  friend std::ostream &operator<<(std::ostream &os, const OverrideEC50Pattern &pattern) {
    os << "[";
    os << pattern.pattern << ", ";
    os << pattern.drug_id << ", ";
    os << pattern.ec50;
    return os << "]";
  }
};

typedef std::vector<OverrideEC50Pattern> OverrideEC50Patterns;


#endif /* TYPEDEF_H */
