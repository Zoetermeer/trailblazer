#include "common.hpp"

/*GLfloat angleBetween(const glm::vec3 &a, const glm::vec3 &b)
{
  auto dotp = glm::dot(a, b);
  auto magA = glm::magnitude(a);
  auto magB = glm::magnitude(b);
  
  return RAD_TO_DEG(acos(dotp / (magA * magB)));
} */

double random1()
{
  return rand() / double(RAND_MAX);
}

double randBetween(double min, double max)
{
  return (max - min) * random1() + min;
}

int randBetween(int min, int max)
{
  return (rand() % ((max - min) + 1)) + min;
}

GLfloat randAngle()
{
  return randBetween(0.0, 360.0);
}

bool percentChance(float pct)
{
  double v = randBetween(0.0, 100.0);
  return v <= pct;
}

glm::vec3 randVec3(GLfloat min, GLfloat max)
{
  glm::vec3 v(randBetween(min, max), randBetween(min, max), randBetween(min, max));
  return v;
}

std::string randomName(bool upper)
{
  std::string str;
  int i, nchars = randBetween(4, 10);
  bool consonant = percentChance(50.0);
  for (i = 0; i < nchars; i++) {
    if (consonant)
      str += randConsonant(upper);
    else
      str += randVowel(upper);
    
    consonant = !consonant;
  }
  
  return str;
}

char randVowel(bool upper)
{
  char ret;
  static char vowels[] = { 'a', 'e', 'i', 'o', 'u' };
  ret = vowels[randBetween(0, (sizeof(vowels) / sizeof(char)) - 1)];
  if (upper)
    return ret & 223;
  
  return ret;
}

char randConsonant(bool upper)
{
  char ret;
  static char cs[] =
  {
    'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    'm', 'n', 'p', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z'
  };
  
  ret = cs[randBetween(0, (sizeof(cs) / sizeof(char)) - 1)];
  if (upper)
    return ret & 223;
  
  return ret;
}

GLfloat nextAngle(GLfloat cur, GLfloat incr)
{
  GLfloat ang = cur + incr;
  if (ang < 0.0)
    ang = 360.0 + ang;
  else if (ang > 360.0) 
    ang -= 360.0;

  return ang;
}
