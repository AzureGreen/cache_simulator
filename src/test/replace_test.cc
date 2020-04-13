
#include <iostream>
#include <include/defs.h>
#include <vector>

#include "simulate/replace.h"

int main() {
  const char * ip = "172.24.72.39";
  size_t dt = 20200223;
  int bucket_id = 1;
  std::vector<std::string> algor_names = {
    // "FIFO", "LRU", "S3LRU"
    "FIFO2"
  };
  size_t size = 100 * constant::GIGABYTE;
  size_t task_id = 4;
  Performance p = {0};
  for (auto &algor_name : algor_names) {
    for (int i = 0; i < bucket_id; i++) {
      for (size_t j = dt; j < 20200230; j++) {
        CacheTest(task_id, ip, std::to_string(j).data(), i, algor_name.data(), size, &p);
        std::cout << algor_name << " " << p.hit_bytes << " " << p.request_bytes << " " 
          << 1.0 * p.hit_bytes / p.request_bytes << std::endl;
      }
    }
  }
  return 0;
}
