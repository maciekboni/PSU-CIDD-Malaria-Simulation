//
// Created by Pakhi on 3/1/2021.
//
//Spatial/Location Test
#include "Spatial/Location.h"
#include "gtest/gtest.h"

using namespace Spatial;

TEST(LocationTest, LocTest) {
Location loc(1, 0.123, 3.456, 10000);

ASSERT_EQ(1, loc.id);
ASSERT_EQ(0,loc.beta);
ASSERT_EQ(10000,loc.population_size);
//ASSERT_EQ(Approx(0.123),loc.coordinate->latitude);
//ASSERT_EQ(3.456,loc.coordinate->longitude);

}
