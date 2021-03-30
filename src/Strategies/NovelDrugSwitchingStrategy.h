#ifndef NOVELDRUGSWITCHINGSTRATEGY_H
#define NOVELDRUGSWITCHINGSTRATEGY_H

#include <vector>
#include "IStrategy.h"
#include "Core/PropertyMacro.h"
#include "NestedMFTStrategy.h"

class NovelDrugSwitchingStrategy : public NestedMFTStrategy {
DISALLOW_COPY_AND_ASSIGN(NovelDrugSwitchingStrategy)

DISALLOW_MOVE(NovelDrugSwitchingStrategy)

public:
  bool is_switched{false};
  int switch_to{0};
  double replace_fraction{0.0};
  int replace_duration{0};
  double tf_threshold{0.1};

  NovelDrugSwitchingStrategy();

  virtual ~NovelDrugSwitchingStrategy() = default;

  std::string to_string() const override;

  void monthly_update() override;
};

#endif // NOVELDRUGSWITCHINGSTRATEGY_H
