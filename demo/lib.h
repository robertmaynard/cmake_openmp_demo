//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE for details.
//============================================================================
#ifndef demo_lib_h
#define demo_lib_h

#include <array>
#include <utility>

namespace demo {

struct bounds;

float sum(std::vector<float> const& values);

bounds min_max(std::vector<float> const& values);

struct bounds
{
  constexpr float operator[](std::ptrdiff_t idx) const  { return m_array[idx]; }
  float& operator[](std::ptrdiff_t idx) { return m_array[idx]; }

  bounds &operator+=(const bounds &other) {
    if (other[0] < m_array[0]) {
      m_array[0] = other[0];
    }
    if (other[1] > m_array[1]) {
      m_array[1] = other[1];
    }
    return *this;
  }
  std::array<float,2> m_array;
};

}

#endif
