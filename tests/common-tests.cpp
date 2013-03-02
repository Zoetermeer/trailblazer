#include "common-tests.hpp"
#include "../common.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(CommonTests);

void CommonTests::randTest1()
{
  srand(time(NULL));
  bool found1 = false, found0 = false;
  for (int i = 0; i < 100; i++) {
    int x = randBetween(0, 1);
    if (!x) found0 = true;
    else if (x == 1) found1 = true;
    else CPPUNIT_FAIL("Found something other than 0 or 1!");
  }
  
  if (!found1 || !found0)
    CPPUNIT_FAIL("Didn't get either a 0 or 1!");
}