/*
 * File:   IntParasiteDatabase.h
 * Author: Merlin
 *
 * Created on March 18, 2014, 3:06 PM
 */

#ifndef INTPARASITEDATABASE_H
#define INTPARASITEDATABASE_H

#include <map>

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Genotype.h"

class Genotype;
class Config;

typedef std::map<ul, Genotype*> GenotypePtrMap;

class GenotypeDatabase : public GenotypePtrMap {
  DISALLOW_COPY_AND_ASSIGN(GenotypeDatabase)

  DISALLOW_MOVE(GenotypeDatabase)

//  VIRTUAL_PROPERTY_REF(MatingMatrix, mating_matrix)

  VIRTUAL_PROPERTY_REF(IntVector, weight)

public:
  std::map<std::string, Genotype*> aa_sequence_id_map;

public:
  GenotypeDatabase();

  virtual ~GenotypeDatabase();

  void add(Genotype* genotype);

  Genotype* get_genotype(const std::string& aa_sequence, Config* pConfig);

  unsigned int get_id(const std::string& aa_sequence, Config* config);

  Genotype* get_genotype_from_alleles_structure(const IntVector& alleles);

private:
  unsigned int auto_id {0};
};

#endif /* INTPARASITEDATABASE_H */
