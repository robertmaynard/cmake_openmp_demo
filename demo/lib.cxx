//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE for details.
//============================================================================

#include <limits>
#include <omp.h>
#include <vector>

#include <iostream>

#include <demo/lib.h>

namespace demo {

float sum(std::vector<float> const &values) {

  std::vector<float> buffer(omp_get_max_threads(), 0);

  std::size_t end = values.size();

// #pragma omp taskloop shared(values)
#pragma omp parallel for shared(values, end)
  for (std::size_t i = 0; i < end; ++i) {
    buffer[omp_get_thread_num()] += values[i];
  }

  float total = 0;
  for (auto &t : buffer) {
    total += t;
  }
  return total;
}

bounds min_max(std::vector<float> const &values) {

  bounds total = {std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::lowest()};
  std::vector<bounds> buffer(omp_get_max_threads(), total);

  std::size_t end = values.size();

  // #pragma omp taskloop simd shared(b, values)
#pragma omp parallel for shared(values, end)
  for (std::size_t i = 0; i < end; ++i) {
    auto v = values[i];
    auto &b = buffer[omp_get_thread_num()];
    if (b[0] > v) {
      b[0] = v;
    }
    if (b[1] < v) {
      b[1] = v;
    }
  }

  for (auto &b : buffer) {
    if (total[0] > b[0]) {
      total[0] = b[0];
    }
    if (total[1] < b[1]) {
      total[1] = b[1];
    }
  }

  return total;
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
