/**
 * File              : lru.h
 * Author            : AuzreGreen <zhangjiawei1006@gmail.com>
 * Date              : 28.08.2017
 * Last Modified Date: 03.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CACHE_TINY_LRU_H
#define CACHE_TINY_LRU_H

#include "cache/base/cache.h"

#include <list>
#include <unordered_map>
#include <cstddef> //size_t

namespace tinylfu {
template<class K>
class LRUCache : public Cache<K> {
 public:
  LRUCache(size_t capacity)
    : Cache<K>(LRU, capacity) {}

  ~LRUCache() {
    lru_.clear();
    cache_map_.clear();
  }

  K victim_key() const { return lru_.back().key; }

  bool get(K key, size_t &size) {
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()) {
      size = it->second->size;
      // move current file to MRU positition
      lru_.splice(lru_.begin(), lru_, it->second);
      return true;
    }
    return false;
  }

  bool insert(K key, size_t size) {
    size_t chunk_size = chunksize(size);
    while (cache_size_ + chunk_size > cache_capacity_) {
      evict();
    }
    // insert to mru
    lru_.emplace_front(key, size);
    cache_map_[key] = lru_.begin();
    cache_size_ += chunk_size;
    return true;
  }

 private:
  struct LRUNode {
    K key;
    size_t size;
    explicit LRUNode(K _key, size_t _size) : key(_key), size(_size) {}
  };

  using LRUNodeIter = typename std::list<LRUNode>::iterator;

 public:
  LRUNode evict () {
    LRUNodeIter node = --lru_.end();
    cache_size_ -= chunksize(node->size);
    cache_map_.erase(node->key);
    // delete lru
    // before delete, save the victim node
    auto ret = *node;
    lru_.erase(node);
    return ret;
  }

 private:
  
  using Cache<K>::cache_size_;
  using Cache<K>::cache_capacity_;

  using Cache<K>::chunksize;

  std::unordered_map<K, LRUNodeIter> cache_map_;
  std::list<LRUNode> lru_;
  
};
};
#endif
