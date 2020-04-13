/**
 * File              : max_bhr_test.cc
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 17.03.2020
 * Last Modified Date: 17.03.2020
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#include <iostream>
#include <include/defs.h>

#include "statistic/max_bhr.h"

int main() {
  const char * ip = "group";
  int bucket_id = 0;
  size_t task_id = 2;
  Metrics metrics = {0};
  int dt = 20200321;

  // for (int dt = 20200223; dt < 20200230; dt++) {
    Statistic(task_id, ip, std::to_string(dt).data(), bucket_id, &metrics);
    std::cout << metrics.uniq_bytes << " "
    << metrics.req_bytes << " " 
    << metrics.tot_uniq_bytes << " " 
    << metrics.tot_req_bytes << " " 
    // << metrics.avg_hy_ratio << " " 
    // << metrics.daily_hy_ratio << std::endl;
    << std::endl;
 // }

  return 0;
}
