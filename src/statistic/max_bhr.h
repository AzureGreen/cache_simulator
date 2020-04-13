//
// Created by AzureGreen on 2020/1/7.
//

#ifndef CDN_CACHE_ANALYSIS_MAX_BHR_H
#define CDN_CACHE_ANALYSIS_MAX_BHR_H

#include <string>
#include <cstdint>

struct Metrics {
  double uniq_bytes;
  double req_bytes;
  double tot_uniq_bytes; 
  double tot_req_bytes;
  // double avg_hy_ratio; 
  // double daily_hy_ratio;
};


extern "C" {

void Statistic(size_t task_id, const char * ip, const char * dt, int bucket_id,
    Metrics *metrics);
};

#endif //CDN_CACHE_ANALYSIS_REPLACE_H
