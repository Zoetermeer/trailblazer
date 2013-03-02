#include "matrix-tests.hpp"
#include "../matrix.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(MatrixTests);

void MatrixTests::setUp()
{
  
}

void MatrixTests::tearDown()
{
  
}

void MatrixTests::identityTests()
{
  Matrix id1 = Matrix::identity();
  Matrix id2 = Matrix::identity();
  CPPUNIT_ASSERT(id1 == id2);
  CPPUNIT_ASSERT(id1 * id2 == id1);
  CPPUNIT_ASSERT(id2 * id1 == id1);
}