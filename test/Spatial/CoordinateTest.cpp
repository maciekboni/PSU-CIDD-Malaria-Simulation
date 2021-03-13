//Spatial/Coordinate Test

#include "gtest/gtest.h"
#include "Spatial/Coordinate.h"
#include <math.h>

using namespace Spatial;

TEST(CoordinateTest, InitializeWithLatAndLong){
    Coordinate c(10, 100);
    ASSERT_EQ(10, c.latitude);
    ASSERT_EQ(100, c.longitude);
    Coordinate c1;
    ASSERT_EQ(0, c1.latitude);
    ASSERT_EQ(0,c1.longitude);
}

TEST(CoordinateTest, CalculateDistanceInKm){
    Coordinate c1;
    Coordinate c2(80, 70);
    ASSERT_EQ(9629,round(Coordinate::calculate_distance_in_km(c1, c2)));
}
