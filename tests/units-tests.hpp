#ifndef UNITS_TESTS_H
#define UNITS_TESTS_H

#include <gtest/gtest.h>
#include "printer.hpp"
#include "../units.hpp"

TEST(UnitsTests, NoiseToFeetNeverExceedsMaxHeight) {
  for (float n = -1.0; n < 2.0; n += 0.1) {
    EXPECT_LE(Units::noiseToFeet(n), MAX_HEIGHT_IN_FEET * 2);
  }
}

#endif