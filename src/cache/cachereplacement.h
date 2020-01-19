/**
 * File              : cachereplacement.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 14.08.2019
 * Last Modified Date: 03.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CORE_REPLACE_H
#define CORE_REPLACE_H

#include "cache/cacheinstance.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

#include "util/parse/feature_parser.h"
#include "util/util.h"


using CacheKey = size_t;

// test routine 
class CacheTester {

 public:

  CacheTester(algorithm_t algor, size_t cap)
    : instance_(algor, cap) {}

  void OutTotalResult() {
    int max_digits = std::numeric_limits<long double>::max_digits10;
    std::cout << instance_.Capacity() << ","
      << std::setprecision(max_digits)
      << instance_.ObjectHitRate() << ","
      << instance_.ByteHitRate() << ","
      << instance_.WriteRate() << ","
      << instance_.TotalRequestBytes() << ","
      << instance_.TotalHitBytes() << ","
      << instance_.TotalRequestObjects() << ","
      << instance_.TotalHitObjects() << std::endl;
  }

  void RunNormal(FeatureParser &parser) {
    while (parser.ParseLine()) {
      if (parser.file_size() == 0) {
        continue;
      }
      if (!instance_.Get(parser.key(), parser.size())) {
        if (!instance_.Admit(parser.key())) continue;
        instance_.Insert(parser.key(), parser.size());
      }
    }
  }

  void RunOpt(FeatureParser &parser, FileReader &nextposfs) {
    size_t nextpos = 0;
    while (parser.ParseLine()) {
      nextposfs >> nextpos;
      if (parser.file_size() == 0) {
        continue;
      }
      if (!instance_.Get(parser.key(), parser.size(), nextpos)) {
        instance_.Insert(parser.key(), parser.size(), nextpos);
      }
    }
  }  
  
  void Reset(bool warmup) {
    instance_.Reset(warmup);
  }

  void Run(FeatureParser &parser, FileReader &nextposfs) { 
    return instance_.CacheName() == OPT ? RunOpt(parser, nextposfs) : RunNormal(parser);
  }

 private :
  CacheInstance<CacheKey> instance_;
};


#endif


