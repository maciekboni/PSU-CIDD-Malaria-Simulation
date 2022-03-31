/*
 * File:   ParasitePopulation.h
 * Author: Merlin
 *
 * Created on July 11, 2013, 1:53 PM
 */

#ifndef SINGLEHOSTCLONALPARASITEPOPULATIONS_H
#define SINGLEHOSTCLONALPARASITEPOPULATIONS_H

#include <vector>

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Population/ClonalParasitePopulation.h"
class ClonalParasitePopulation;

class Person;

class DrugType;

class DrugsInBlood;

class ParasiteDensityUpdateFunction;

class SingleHostClonalParasitePopulations {
  OBJECTPOOL(SingleHostClonalParasitePopulations)

  DISALLOW_COPY_AND_ASSIGN(SingleHostClonalParasitePopulations)

  POINTER_PROPERTY(Person, person)

  POINTER_PROPERTY(std::vector<ClonalParasitePopulation *>, parasites)

public:
  // this value will be automatically updated daily in the function clear_cured_parasites
  // in order to have accurate sum of all density
  double log10_total_infectious_denstiy{ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY};

public:
  SingleHostClonalParasitePopulations(Person *person = nullptr);

  //    ParasitePopulation(const ParasitePopulation& orig);
  virtual ~SingleHostClonalParasitePopulations();

  void init();

  virtual int size();

  virtual void add(ClonalParasitePopulation *blood_parasite);

  virtual void remove(ClonalParasitePopulation *blood_parasite);

  virtual void remove(const int &index);

  virtual int latest_update_time() const;

  virtual bool contain(ClonalParasitePopulation *blood_parasite);

  void change_all_parasite_update_function(ParasiteDensityUpdateFunction *from,
                                           ParasiteDensityUpdateFunction *to) const;

  void update();

  void clear_cured_parasites();

  void clear();

  void update_by_drugs(DrugsInBlood *drugs_in_blood) const;

  bool has_detectable_parasite() const;

  bool is_gametocytaemic() const;
};

#endif /* SINGLEHOSTCLONALPARASITEPOPULATIONS_H */
