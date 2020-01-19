/**
 * File              : clock.h
 * Author            : AuzreGreen <zhangjiawei1006@gmail.com>
 * Date              : 28.08.2019
 * Last Modified Date: 15.08.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef BASE_CLOCK_H
#define BASE_CLOCK_H

#include "cache.h"

#include <list>
#include <unordered_map>
#include <cstddef> //size_t

template<class K>
class CLOCKCache : public Cache<K> {
 public:
  CLOCKCache(size_t capacity, size_t chunk)
    : Cache<K>(CLOCK, capacity, chunk)
    , clock_iter_(clock_.begin()) {}

  ~CLOCKCache() {
    cache_map_.clear();
    clock_.clear();
  }

  bool get(K key, size_t &size) {
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()) {
      CLOCKNodeIter node_iter = it->second;
      size = node_iter->size;
      node_iter->tag = true;  // reaccess, set true
      return true;
    }
    return false;
  }

  bool insert(K key, size_t size) {
    while (cache_size_ + chunk_size_ > cache_capacity_) {
      evict();
    }
    cache_map_[key] = clock_.emplace(clock_iter_, key, size);
    cache_size_ += chunk_size_;
    return true;
  }

 private:
  struct CLOCKNode {
    K key;
    size_t size;
    bool tag; // the mark to decide wheter evict or not.
    explicit CLOCKNode(K key, size_t size) 
      : key(key), size(size), tag(false) {}
  };

  using CLOCKNodeIter = typename std::list<CLOCKNode>::iterator;

  void CheckRing(CLOCKNodeIter& it) {
    if (it == clock_.end()) {
      it = clock_.begin();
    }
  }

  void NextIter(CLOCKNodeIter& it) {
    CheckRing(++it);
  }

  void evict() {
    CheckRing(clock_iter_);
    auto it = clock_iter_; 
    do {
      // travel like a ring
      if (it->tag) {
        it->tag = false;
      } else {
        clock_iter_ = it;
        break;
      }
      NextIter(it);  // Navigate to the next node  
    } while (it != clock_iter_);

    // current iter will be erase soon, so save next iter.
    NextIter(clock_iter_);    
    cache_size_ -= chunk_size_;
    cache_map_.erase(it->key);
    // delete clock
    clock_.erase(it);
  }

  using Cache<K>::cache_size_;
  using Cache<K>::cache_capacity_;
  using Cache<K>::chunk_size_;

  std::unordered_map<K, CLOCKNodeIter> cache_map_;
  std::list<CLOCKNode> clock_;
  CLOCKNodeIter clock_iter_;
  
};
#endif
