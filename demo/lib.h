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

using bounds = std::array<float,2>;

float sum(std::vector<float> const& values);

bounds min_max(std::vector<float> const& values);

std::pair<float, bounds> sum_and_min_max(std::vector<float> const& values);

}

#endif
