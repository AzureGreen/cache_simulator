/**
 * File              : cache_checkpoint.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 03.09.2019
 * Last Modified Date: 19.01.2020
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CACHE_CHECKPOINT_H
#define CACHE_CHECKPOINT_H

#include "cacheinstance.h"

#include <memory>
#include <cstring>

#include "cache/base/cache.h"
#include "cache/checkpoint/clru.h"
#include "cache/checkpoint/cfifo.h"
#include "util/fileio/file_reader.h"
#include "util/fileio/file_writer.h"

template <typename T>
class CacheCheckpoint {
 public:
  CacheCheckpoint(algorithm_t algor, size_t capacity) {
    cache_ = std::unique_ptr<Cache<T>>(CreateCache(algor, capacity));
  }

  CacheCheckpoint(CacheCheckpoint &&cache) noexcept
      : cache_(cache.cache_) {
    memcpy(&this->cm_, &cache.cm_, sizeof cache.cm_);
    memset(&cache.cm_, 0, sizeof cache.cm_);
    cache.cache_ = nullptr;
  }

  virtual ~CacheCheckpoint() {}

  // daily

  inline long double FluxOut() const { return cm_.sb; }

  inline long double FluxHit() const { return cm_.hb; }

  inline size_t RequestNum() const { return cm_.so; }

  inline size_t RequestHit() const { return cm_.ho; }

  inline algorithm_t CacheName() const { return cache_->name(); }

  inline size_t Capacity() const { return cache_->capacity(); }

  Cache<T> operator*() const { return *cache_; }

  bool Get(T key, size_t size) {
    bool ret = cache_->get(key, size);
    if (ret) {
      cm_.ho ++;
      cm_.hb += size;
    }
    // sum of request objects/bytes
    cm_.so ++;
    cm_.sb += size;
    return ret;
  }

  bool Insert(T key, size_t size) {
    cm_.wo ++;
    // cm_.wb += chunksize;
    return cache_->insert(key, size);
  }

  // this is for opt
  // opt need nextpos
  bool Get(T key, size_t size, size_t nextpos) {
    bool ret = cache_->get(key, size, nextpos);
    if (ret) {
      cm_.ho ++;
      cm_.hb += size;
    }
    // maybe overflow 64bit
    cm_.so ++;
    cm_.sb += size;
    return ret;
  }

  bool Insert(T key, size_t size, size_t nextpos) {
    cm_.wo ++;
    return cache_->insert(key, size, nextpos);
  }

  void LoadCheckpoint(FileReader &reader) {
    while (!reader.eof()) {
      std::string &&line = reader.ReadLine();
      if (reader.fail()) break;
      cache_->Push(line);
    }
  }

  void SaveCheckpoint(FileWriter &writer) {
    std::string line = cache_->Pop();
    do {
      writer.WriteLine(line);
      line = cache_->Pop();
    } while (line != "");
  }

 private:
  virtual Cache<T>* CreateCache(algorithm_t algor, size_t cache_capacity) {
    switch(algor) { 
      case LRU: 
        return new LRUCheckPoint<T>(cache_capacity);
      case FIFO:
        return new FIFOCheckPoint<T>(cache_capacity);
       default:
        return nullptr;
    }
  }

  CacheMetrics cm_;  // performance metrics
  std::unique_ptr<Cache<T>> cache_;
};

#endif
