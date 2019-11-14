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

  std::size_t end = values.size();
  float total = 0;

#pragma omp parallel for shared(values, end) reduction(+:total)
  for (std::size_t i = 0; i < end; ++i) {
    total += values[i];
  }

  return total;
}

#pragma omp declare reduction(BoundsMinMax: bounds: omp_out += omp_in)

bounds min_max(std::vector<float> const &values) {

  bounds total = {std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::lowest()};
  std::size_t end = values.size();
  if(end == 0) {
    return total;
  } else if(end == 1) {
    return bounds { values[0], values[0] };
  }

#pragma omp parallel for shared(values, end) reduction(BoundsMinMax:total)
  for (std::size_t i = 0; i < end; ++i) {
    total += bounds{values[i], values[i]};
  }

  return total;
}

} // namespace demo
