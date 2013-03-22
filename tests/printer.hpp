#ifndef PRINTER_H
#define PRINTER_H

#include <iostream>

namespace glm {
  namespace detail {
    void PrintTo(const ivec3& v3, ::std::ostream* os) {
      *os << "(x=" << v3.x << ",y=" << v3.y << ",z=" << v3.z << ")";
    }
    
    void PrintTo(const vec3& v3, ::std::ostream* os) {
      *os << "(x=" << v3.x << ",y=" << v3.y << ",z=" << v3.z << ")";
    }
    
    void PrintTo(const vec4 &v4, ::std::ostream *os) {
      *os << "(x=" << v4.x << ",y=" << v4.y << ",z=" << v4.z << ",w=" << v4.w << ")";
    }
  }
}

#endif