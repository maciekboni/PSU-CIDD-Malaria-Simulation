//
// Created by Pakhi on 3/1/2021.
//

#include "Spatial/Location.h"
#include "gtest/gtest.h"
#include "math.h"

using namespace Spatial;

TEST(LocationTest, LocTest) {
Location loc(1, 0.123, 3.456, 10000);

EXPECT_EQ(1, loc.id);
EXPECT_EQ(0,loc.beta);
EXPECT_EQ(1000,loc.population_size);
EXPECT_EQ(ceil(0.123),loc.coordinate->latitude);
EXPECT_EQ(ceil(3.456),loc.coordinate->longitude);

}
