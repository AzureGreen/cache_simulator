/**
 * File              : larc.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 26.12.2019
 * Last Modified Date: 26.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CACHE_LARC_H
#define CACHE_LARC_H

#include "cache.h"
#include "lru.h"
#include "larcghost.h"

#include <list>
#include <unordered_map>
#include <cstddef> //size_t

template<class K>
class LARCCache : public Cache<K> {
 public:
  LARCCache(size_t capacity)
    : Cache<K>(LARC, capacity),
      lru_(capacity),
      ghost_(capacity) {}
  ~LARCCache() {}

  virtual bool get(K key, size_t &size) { 
    bool ret = lru_.get(key, size); 
    if (ret) ghost_.DoHit();
    return ret;
  }

  virtual bool admit(K key) { 
    ghost_.DoMiss();
    return !ghost_.IsStreamFile(key);
  }

  virtual bool insert(K key, size_t size) {
    return lru_.insert(key, size);    
  }

 private:
  LRUCache<K> lru_;
  LARCGhost<K> ghost_;
};
#endif


