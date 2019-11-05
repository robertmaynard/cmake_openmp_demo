//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE for details.
//============================================================================

#include <limits>
#include <omp.h>
#include <vector>

#include <demo/lib.h>

namespace demo {

float sum(std::vector<float> const &values) {

  float total = 0;
  std::size_t end = values.size();

#pragma omp taskloop simd shared(total, values)
  for (std::size_t i = 0; i < end; ++i) {
    total += values[i];
  }
  return total;
}

bounds min_max(std::vector<float> const &values) {

  bounds b = {std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest()};
  std::size_t end = values.size();

#pragma omp taskloop simd shared(b, values)
  for (std::size_t i = 0; i < end; ++i) {
    auto v = values[i];
    if (b[0] > v) {
      b[0] = v;
    }
    if (b[1] < v) {
      b[1] = v;
    }
  }
}

std::pair<float, bounds> sum_and_min_max(std::vector<float> const &values) {

  std::pair<float, bounds> result;
#pragma omp single
  {
#pragma omp task
    result.first = sum(values);

#pragma omp task
    result.second = min_max(values);
  }
  return result;
}
} // namespace demo
