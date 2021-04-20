#ifndef NOVELDRUGSWITCHINGSTRATEGY_H
#define NOVELDRUGSWITCHINGSTRATEGY_H

#include <vector>
#include "IStrategy.h"
#include "Core/PropertyMacro.h"
#include "NestedMFTStrategy.h"

class NovelDrugIntroductionStrategy : public NestedMFTStrategy {
DISALLOW_COPY_AND_ASSIGN(NovelDrugIntroductionStrategy)

DISALLOW_MOVE(NovelDrugIntroductionStrategy)

public:
  bool is_switched{false};
  int newly_introduced_strategy_id{0};
  double replacement_fraction{0.0};
  int replacement_duration{0};
  double tf_threshold{0.1};

  NovelDrugIntroductionStrategy();

  virtual ~NovelDrugIntroductionStrategy() = default;

  std::string to_string() const override;

  void monthly_update() override;
};

#endif // NOVELDRUGSWITCHINGSTRATEGY_H
