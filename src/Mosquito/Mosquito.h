//
// Created by nguyentd on 3/11/2022.
//

#ifndef POMS_SRC_MOSQUITO_MOSQUITO_H
#define POMS_SRC_MOSQUITO_MOSQUITO_H
#include "Core/PropertyMacro.h"

class Model;

class Mosquito {
public:
  Mosquito(const Mosquito &) = delete;
  const Mosquito &operator=(const Mosquito &) = delete;

public:
  Model *model { nullptr };

public:
  Mosquito(Model *model = nullptr);
  virtual ~Mosquito() = default;

  void init();
};

#endif  // POMS_SRC_MOSQUITO_MOSQUITO_H
