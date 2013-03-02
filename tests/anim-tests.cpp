#include "anim-tests.hpp"
#include "../animation.hpp"
#include "../common.hpp"
#include "../process.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(AnimTests);

void AnimTests::setUp()
{
  
}

void AnimTests::tearDown()
{
  
}

void AnimTests::animTest1()
{
  Attitude att;
  Animation a(Interpolation::Linear, att.yaw, 10.f, 2000);
  
  GLfloat prev, cur;
  for (unsigned int d = 10; d <= 2000; d += 10) {
    prev = att.yaw;
    a.advance(10);
    CPPUNIT_ASSERT_EQUAL(d, a.getElapsedTime());
    if (a.getElapsedTime() < 2000)
      CPPUNIT_ASSERT(a.getNormalizedTimeElapsed() < 1.f);
    
    cur = att.yaw;
    CPPUNIT_ASSERT((GLfloat)att.yaw > prev);
    CPPUNIT_ASSERT_EQUAL((GLfloat)att.yaw, cur);
  }
  
  CPPUNIT_ASSERT(a.isDone());
  CPPUNIT_ASSERT_EQUAL((GLfloat)att.yaw, 10.f);
  
  Animation b(Interpolation::Linear, att.yaw, -10.f, 2000);
  b.advance(2000);
  CPPUNIT_ASSERT(b.isDone());
  CPPUNIT_ASSERT_EQUAL((GLfloat)att.yaw, 0.f);
}

void AnimTests::animTest2()
{
  Attitude att;
  Animation a(Interpolation::QuadraticEaseIn, att.yaw, 4.f, 4);
  for (unsigned int d = 0; d < 4; d++) {
    GLclampf nt = a.getNormalizedTimeElapsed();
    switch (d)
    {
      case 0:
        CPPUNIT_ASSERT(nt == 0.);
        break;
      case 1:
        CPPUNIT_ASSERT(nt == 0.25);
        break;
      case 2:
        CPPUNIT_ASSERT(nt == 0.50);
        break;
      case 3:
        CPPUNIT_ASSERT(nt == 0.75);
        break;
    }
    
    a.advance(1);
  }
  
  CPPUNIT_ASSERT_EQUAL(a.getNormalizedTimeElapsed(), 1.f);
  CPPUNIT_ASSERT_EQUAL(4.f, (GLfloat)att.yaw);
}

void AnimTests::animTest3()
{
  Attitude att;
  Animation a(Interpolation::Linear, att.yaw, 50.f, 500);
  ProcessList::add(&a);
  while (!a.isDone())
    ProcessList::advanceAll(1000 / 60); //Advance ~60FPS
  
  //If the test never terminates, bug!
  CPPUNIT_ASSERT_EQUAL(1.f, a.getNormalizedTimeElapsed());
}

void AnimTests::easeTest1()
{
  Attitude att;
  Animation a(Interpolation::QuadraticEaseInOut, att.yaw, 100.f, 100);
  for (unsigned int d = 0; d <= 100; d += 10) {
    CPPUNIT_ASSERT_EQUAL(a.getElapsedTime(), d);
    a.advance(10);
  }
  
  CPPUNIT_ASSERT(a.isDone());
  CPPUNIT_ASSERT_EQUAL((GLfloat)att.yaw, 100.f);
}




