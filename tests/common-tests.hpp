#ifndef COMMON_TESTS_H
#define COMMON_TESTS_H

#include <cppunit/extensions/HelperMacros.h>

class CommonTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(CommonTests);
  CPPUNIT_TEST(randTest1);
  CPPUNIT_TEST_SUITE_END();
  
public:
  void randTest1();
};

#endif