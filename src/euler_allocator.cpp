#include "const.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

using namespace std::chrono;

static constexpr double epsilon = 1.0e-6;

std::vector<double> step(const std::vector<double>& x) {
  std::vector<double> res(2);
  res[0] = x[0] + epsilon * x[1];
  res[1] = x[1] + epsilon * std::sin(x[0]);
  return res;
}

int main() {
  std::cout << "euler allocator\n";
  std::vector<double> x = {0.1, 0};
  auto start = high_resolution_clock::now();
  for (int i = 0; i < ITERATIONS; i++) {
    x = step(x);
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  std::cout << (duration.count() * 1.0e-6) << " seconds\n";
  std::cout << x[0] << "," << x[1] << std::endl << std::endl;
}