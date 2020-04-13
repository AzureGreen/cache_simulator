/**
 * File              : cacheinstace.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 03.09.2019
 * Last Modified Date: 03.09.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CACHE_CACHEINSTANCE_H
#define CACHE_CACHEINSTANCE_H

#include "cache/base/cache.h"
#include "cache/base/opt.h"
#include "cache/base/arc.h"
#include "cache/base/lirs.h"
#include "cache/base/slru.h"
#include "cache/base/lru.h"
#include "cache/base/fifo2.h"
#include "cache/base/fifo.h"
#include "cache/base/2q.h"
#include "cache/base/mq.h"
#include "cache/base/tinylfu.h"
#include "cache/base/larc.h"

#include <memory>
#include <cstring>


struct CacheMetrics {
  size_t ho = 0;   // hit objects
  size_t so = 0;   // sum of objects
  size_t wo = 0;   // write objects
  size_t hb = 0; // hit bytes
  // long double wb = 0.0; // write bytes
  size_t sb = 0;  // sum of bytes
};

template <typename K>
class CacheInstance {
 public:
  CacheInstance(algorithm_t algor, size_t capacity) {
    cache_ = std::unique_ptr<Cache<K>>(CreateCache(algor, capacity));
  }

  CacheInstance(CacheInstance &&cache) noexcept
    : cache_(cache.cache_) {
    memcpy(&this->cm_, &cache.cm_, sizeof cache.cm_);
    memset(&cache.cm_, 0, sizeof cache.cm_);
    memcpy(&this->total_cm_, &cache.total_cm_, sizeof cache.total_cm_);
    memset(&cache.total_cm_, 0, sizeof cache.total_cm_);
    cache.cache_ = nullptr;
  }

  virtual ~CacheInstance() {}

  // const CacheMetrics Metrics() const { return cm_; }

  // total

  inline double ObjectHitRate() const { return 1.0 * total_cm_.ho / total_cm_.so; }

  inline double ByteHitRate() const { return 1.0 * total_cm_.hb / total_cm_.sb; }

  inline double WriteRate() const { return 1.0 * total_cm_.wo / total_cm_.so; }

  inline size_t TotalRequestBytes() const { return total_cm_.sb; }
  
  inline size_t TotalHitBytes() const { return total_cm_.hb; }

  inline size_t TotalRequestObjects() const { return total_cm_.so; }

  inline size_t TotalHitObjects() const { return total_cm_.ho; }

  // daily

  inline long double FluxOut() const { return cm_.sb; }

  inline long double FluxHit() const { return cm_.hb; }

  inline size_t RequestNum() const { return cm_.so; }

  inline size_t RequestHit() const { return cm_.ho; }

  inline algorithm_t CacheName() const { return cache_->name(); }

  inline size_t Capacity() const { return cache_->capacity(); }

//  inline size_t ChunkSize() const { return cache_->ChunkSize(); }

  void Reset(bool warmup) { 
    if (!warmup) {
    // update total metrics' value
      total_cm_.ho += cm_.ho;
      total_cm_.wo += cm_.wo;
      total_cm_.so += cm_.so;
      total_cm_.hb += cm_.hb;
      // total_cm_.wb += cm_.wb;
      total_cm_.sb += cm_.sb;
    }
    memset(&cm_, 0, sizeof cm_); 
  } 

  // std::unique_ptr<Cache<K>> GetCache() const { return cache_; }

  Cache<K> operator*() const { return *cache_; }

  bool Get(K key, size_t size) { 
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

  bool Insert(K key, size_t size) { 
    cm_.wo ++;
    // cm_.wb += chunksize;
    return cache_->insert(key, size);
  }

  // this is for opt
  // opt need nextpos 
  bool Get(K key, size_t size, size_t nextpos) { 
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

  bool Insert(K key, size_t size, size_t nextpos) { 
    cm_.wo ++;
    return cache_->insert(key, size, nextpos);
  }

  bool Admit(K key) const { return cache_->admit(key); }

 private:
  virtual Cache<K>* CreateCache(algorithm_t algor, size_t cache_capacity) {
    switch(algor) { 
      case LRU: 
        return new LRUCache<K>(cache_capacity); 
      case SLRU:
        return new SLRUCache<K>(2, cache_capacity); 
      case S3LRU:
        return new SLRUCache<K>(3, cache_capacity); 
      case S4LRU:
        return new SLRUCache<K>(4, cache_capacity); 
       case ARC:
        return new ARCCache<K>(cache_capacity); 
      case LIRS:
        return new LIRSCache<K>(cache_capacity); 
      case FIFO:
        return new FIFOCache<K>(cache_capacity); 
      case FIFO2:
        return new FIFO2Cache<K>(cache_capacity); 
      case TQ:
        return new TQCache<K>(cache_capacity); 
/*       case CLOCK: */
        // return new CLOCKCache<K>(cache_capacity); 
      case MQ:
        // need queue num, lifetime
        // s1 : 64 * 1024
        // soc1 : 2 * 1024 * 1024
        // soc1_vip : 16 * 1024 * 1024
        // soc1_down : 256 * 1024 
        // soc1_hy : 4 * 1024 * 1024 
        return new MQCache<K>(8, 4 * 1024 * 1024, cache_capacity); 
        // return new MQCache<K>(8, 256 * 1024, cache_capacity); 
//      case GDSF:
//        return new GDSFCache<K>(cache_capacity); 
      case OPT:
        return new OPTCache<K>(cache_capacity); 
      case TINYLFU:
        return new TinyLFUCache<K>(cache_capacity);
      case LARC:
        return new LARCCache<K>(cache_capacity);
       default:
        return nullptr;
    }
  }  

  CacheMetrics cm_;  // performance metrics
  CacheMetrics total_cm_;  // performance metrics (save total data)
  std::unique_ptr<Cache<K>> cache_;
};

#endif
