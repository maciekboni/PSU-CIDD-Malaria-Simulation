//
// Created by nguyentd on 1/25/2022.
//

#include "Core/Random.h"
#include "gmock/gmock.h"

class MockRandom : public Random {
public:
  MOCK_METHOD(double, random_uniform, (), (override));
  MOCK_METHOD(unsigned long, random_uniform, (unsigned long range), (override));
};