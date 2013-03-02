#ifndef ANIM_TESTS_H
#define ANIM_TESTS_H

#include <cppunit/extensions/HelperMacros.h>

class AnimTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(AnimTests);
  CPPUNIT_TEST(animTest1);
  CPPUNIT_TEST(animTest2);
  CPPUNIT_TEST(animTest3);
  CPPUNIT_TEST(easeTest1);
  CPPUNIT_TEST_SUITE_END();
  
public:
  void setUp();
  void tearDown();
  
  void animTest1();
  void animTest2();
  void animTest3();
  void easeTest1();
};

#endif