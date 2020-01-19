//
// Created by AzureGreen on 2020/1/7.
//

#ifndef CDN_CACHE_ANALYSIS_REPLACE_H
#define CDN_CACHE_ANALYSIS_REPLACE_H

#include <string>
#include <cstdint>

struct Performance {
  size_t hit_bytes;
  size_t request_bytes;
};


extern "C" {

void CacheTest(const char * ip, const char * dt, int bucket_id,
    const char * algor_name, size_t cache_size, Performance *p);

};

#endif //CDN_CACHE_ANALYSIS_REPLACE_H
