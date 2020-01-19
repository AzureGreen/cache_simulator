
#include <iostream>
#include <include/defs.h>

#include "simulate/replace.h"

int main() {
  const char * ip = "172.20.16.38";
  const char * dt = "20200111";
  int bucket_id = 10;
  const char * algor_name = "FIFO";
  size_t size = 120 * constant::GIGABYTE;
  Performance p = {0};
  for (int i = 0; i < bucket_id; i++) {
    CacheTest(ip, dt, i, algor_name, size, &p);
    std::cout << 1.0 * p.hit_bytes / p.request_bytes << std::endl;
  }

  return 0;
}