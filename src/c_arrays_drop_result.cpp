#include "const.h"
#include <iostream>
#include <chrono>

using namespace std::chrono;

int main() {
  std::cout << "c arrays result\n";
  double a[SIZE];
  double b[SIZE];
  double c[SIZE];
  double d[SIZE];
  double e[SIZE];
  srand(1);
  int m = rand();
  double n = (1.0 + m * 1.0e-9) / (2.0 + m * 1.0e-9);
  for (int j=0; j<SIZE; j++) {
    a[j] = 1/n + j;
  }
  auto start = high_resolution_clock::now();
  for (int i=0; i<ITERATIONS; i++) {
    for (int j=0; j<SIZE; j++) {
      a[j] += j - 1/n;
    }
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  std::cout << "1. " << (duration.count() * 1.0e-6) << " seconds\n";

  start = high_resolution_clock::now();
  for (int i=0; i<ITERATIONS; i++) {
    for (int j=0; j<SIZE; j++) {
      a[j] += n + 1/(j + 1);
      b[j] = a[j] - n;
      c[j] = b[j] - 1/n;
      d[j] = c[j] + n;
      e[j] = d[j] + 1/n;
    }
  }
  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);
  std::cout << "2. " << (duration.count() * 1.0e-6) << " seconds\n";
  double r = 0;
  for (int j=0; j<SIZE; j++) {
    r += a[j]/1000000000;
    r += b[j]/1000000000;
    r -= c[j]/1000000000;
    r -= d[j]/1000000000;
    r += e[j]/1000000000;
  }
  std::cout << std::endl;

  return 0;
}