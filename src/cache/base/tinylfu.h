/**
 * File              : tinylfu.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 26.12.2019
 * Last Modified Date: 26.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CACHE_TINY_LFU_H
#define CACHE_TINY_LFU_H

#include "cache.h"
#include "cache/base/tinylfu/lru.h"
#include "cache/base/tinylfu/slru.h"

#include "cache/base/tinylfu/frequency_sketch.hpp"

#include <list>
#include <unordered_map>
#include <algorithm>
#include <cstddef> //size_t

#include "include/defs.h"

template<class K>
class TinyLFUCache : public Cache<K> {
 public:
  TinyLFUCache(size_t capacity)
    : Cache<K>(LRU, capacity),
      window_(WindowCapcity(capacity)),
      main_(capacity - WindowCapcity(capacity)),
      filter_(capacity / constant::TINY_CHUNK_SIZE) {}

  ~TinyLFUCache() {
  }

  bool get(K key, size_t &size) {
    filter_.record_access(key);
    if (window_.get(key, size)) {
      return true;
    } else {
      return main_.get(key, size);
    }
  }

  bool insert(K key, size_t size) {
    size_t chunk_size = chunksize(size);
    while (window_.size() + chunk_size > window_.capacity()) {
      evict();
    }
    // insert to mru
    window_.insert(key, size);
    return true;
  }

 private:
  
  size_t WindowCapcity(size_t capacity) const { 
    return std::max(constant::MEGABYTE, size_t(std::ceil(0.01f * capacity)));
  }

  void evict() {
    int window_victim_freq = filter_.frequency(window_.victim_key());
    int main_victim_freq = filter_.frequency(main_.victim_key());
    auto node = window_.evict();
    if (window_victim_freq > main_victim_freq) {
      main_.insert(node.key, node.size);
    } 
  }

  using Cache<K>::cache_size_;
  using Cache<K>::cache_capacity_;

  using Cache<K>::chunksize;
  
  tinylfu::LRUCache<K> window_;
  tinylfu::SLRUCache<K> main_;
  frequency_sketch<K> filter_;  

};
#endif
