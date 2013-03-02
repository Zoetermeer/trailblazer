#ifndef MATRIX_TESTS_H
#define MATRIX_TESTS_H

#include <cppunit/extensions/HelperMacros.h>

class MatrixTests : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(MatrixTests);
  CPPUNIT_TEST(identityTests);
  CPPUNIT_TEST_SUITE_END();
  
public:
  void setUp();
  void tearDown();
  
  void identityTests();
};

#endif
