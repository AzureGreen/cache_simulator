/**
 * File              : lru.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 03.12.2019
 * Last Modified Date: 03.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CACHE_LRU_H
#define CACHE_LRU_H

#include "cache/base/cache.h"

#include <list>
#include <unordered_map>
#include <cstddef>

template <typename T>
class LRUCache : public Cache<T> {
 public:
  struct LRUNode {
    T key;
    size_t size;
    explicit LRUNode(T _key, size_t _size) : key(_key), size(_size) {}
  };

  using LRUNodeIter = typename std::list<LRUNode>::iterator;

  explicit LRUCache(size_t capacity)
    : Cache<T>(LRU, capacity) {}

  virtual ~LRUCache() {
    lru_.clear();
    cache_map_.clear();
  }

  inline std::list<struct LRUNode> & lru() { return lru_; }

  inline std::unordered_map<T, typename std::list<struct LRUNode>::iterator>
    & cache_map() { return cache_map_; }

  virtual bool get(T key, size_t &size) {
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()) {
      size = it->second->size;
      // move current file to MRU positition
      lru_.splice(lru_.begin(), lru_, it->second);
      return true;
    }
    return false;
  }

  virtual bool insert(T key, size_t size) {
    size_t chunk_size = this->ChunkSize(size);
    while (this->Full(chunk_size)) {
      evict();
    }
    // insert to mru
    lru_.emplace_front(key, size);
    cache_map_[key] = lru_.begin();
    this->IncreaseSize(chunk_size);
    return true;
  }

 private:

  
  virtual void evict() {
    LRUNodeIter node = std::prev(lru_.end());
    this->DecreaseSize(this->ChunkSize(node->size));
    cache_map_.erase(node->key);
    // delete lru
    lru_.erase(node);
  }

  std::unordered_map<T, LRUNodeIter> cache_map_;
  std::list<LRUNode> lru_;
};
#endif
