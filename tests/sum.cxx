
#include <catch2/catch.hpp>
#include <demo/lib.h>
#include <omp.h>

namespace {
std::vector<float> zero_value;
std::vector<float> one_value = {2.0f};
std::vector<float> two_value = {-2.0f, 2.0f};
std::vector<float> eight_value = {1.0f, 2.0f, 3.0f, 4.0f, 5.0, 6.0f, 7.0f, 8.0f};
} // namespace

TEST_CASE("sum works on simple arrays", "[demo.sum]") {

  REQUIRE(demo::sum(zero_value) == 0.0f);
  REQUIRE(demo::sum(one_value) == 2.0f);
  REQUIRE(demo::sum(two_value) == Approx(0.0f));
  REQUIRE(demo::sum(eight_value) == Approx(36.0f));
}

TEST_CASE("Benchmark sum", "[demo.sum.benchmark]") {
  constexpr std::size_t size = 4194304;
  std::mt19937 rng;
  std::uniform_real_distribution<float> range(-1.0f, 2.0f);
  std::vector<float> values;
  values.resize(size);

  for (std::size_t i = 0; i < size; ++i) {
    auto v = range(rng);
    values[i] = v;
  }

  BENCHMARK("serial sum random vector") {
    float sum = 0;
    for (std::size_t i = 0; i < size; ++i) {
      sum += values[i];
    }
    REQUIRE(sum >= 0);
  };

  BENCHMARK("openmp all cores sum random vector") {
    float sum = demo::sum(values);
    REQUIRE(sum >= 0);
  };

  BENCHMARK("openmp 1 core sum random vector") {
    omp_set_num_threads(1);
    float sum = demo::sum(values);
    REQUIRE(sum >= 0);
  };
}
