/**
 * File              : larcghost.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 17.10.2019
 * Last Modified Date: 17.10.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef BASE_LARCGHOST_H
#define BASE_LARCGHOST_H

#include <list>
#include <unordered_map>
#include <cstddef> //size_t

#include "include/defs.h"

template<class K>
class LARCGhost {
 public:
  LARCGhost(size_t cache_capacity)
    : ghost_size_(0), 
      cache_capacity_(cache_capacity / constant::TINY_CHUNK_SIZE),
      ghost_capacity_(size_t(cache_capacity_ * 0.1f)) {}

  ~LARCGhost() {
    ghost_.clear();
    ghost_map_.clear();
  }

  void DoHit() { DecreaseCapacity(); }

  void DoMiss() { IncreaseCapacity(); }

  virtual bool IsStreamFile(K key) {
    auto it = ghost_map_.find(key);
    if (it != ghost_map_.end()) {
      evict(it->second);
      return false;
    } else {
      insert(key);
      return true;
    }
  }

 private:
  struct LRUNode {
    K key;
    LRUNode(K _key) : key(_key) {}
  };

  using LRUNodeIter = typename std::list<LRUNode>::iterator;

  inline void IncreaseCapacity() {
    ghost_capacity_ = static_cast<size_t>(std::min(0.9 * cache_capacity_,
        ghost_capacity_ + 1.0 * cache_capacity_ / ghost_capacity_));
  }

  inline void DecreaseCapacity() {
    ghost_capacity_ = static_cast<size_t>(std::max(0.1 * cache_capacity_,
        ghost_capacity_ - 1.0 * cache_capacity_ / 
        (cache_capacity_ - ghost_capacity_)));

    // need evict ?
    while (ghost_size_ > ghost_capacity_) {
      evict(--ghost_.end());
    }
  }

  void insert(K key) {
    while (ghost_size_ >= ghost_capacity_) {
      evict(--ghost_.end());
    }
    // insert to mru
    ghost_.emplace_front(key);
    ghost_map_[key] = ghost_.begin();
    ghost_size_++;
  }

  void evict(LRUNodeIter node) {
    ghost_size_--;
    ghost_map_.erase(node->key);
    // delete lru
    ghost_.erase(node);
  }

  // size below are all calculated in item scale
  size_t ghost_size_;
  size_t cache_capacity_;
  size_t ghost_capacity_;

  std::list<LRUNode> ghost_;
  std::unordered_map<K, LRUNodeIter> ghost_map_;

};
#endif


