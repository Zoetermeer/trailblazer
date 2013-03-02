#ifndef EVENT_TESTS_H
#define EVENT_TESTS_H

#include <cppunit/extensions/HelperMacros.h>

class EventTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(EventTests);
  CPPUNIT_TEST(playerMoveTest1);
  CPPUNIT_TEST_SUITE_END();
  
public:
  void setUp();
  void tearDown();
  
  void playerMoveTest1();
};

#endif