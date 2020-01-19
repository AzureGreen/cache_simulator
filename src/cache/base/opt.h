/**
 * File              : opt.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 29.08.2019
 * Last Modified Date: 11.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */
#ifndef CACHE_OPT_H
#define CACHE_OPT_H

#include "cache.h"

#include <unordered_map>
#include <set>
#include <cstddef> //size_t

template <class K>
class OPTCache : public Cache<K> {
public:
  OPTCache(size_t capacity) : Cache<K>(OPT, capacity) {}

  ~OPTCache() {
    rbtree_.clear();
    cache_map_.clear();
  }

  bool get(K key, size_t &size, size_t next_pos) {
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()) {
      size = it->second->size;
      // update rbtree
      rbtree_.erase(it->second);
      it->second = rbtree_.emplace(key, size, next_pos);
      return true;
    } 
    return false;
  }

  bool insert(K key, size_t size, size_t next_pos) {
    size_t chunk_size = chunksize(size);
    while (cache_size_ + chunk_size > cache_capacity_) {
      evict();
    }
    cache_map_[key] = rbtree_.emplace(key, size, next_pos);
    cache_size_ += chunk_size;

    return true;
  }

 private:
  struct OPTNode {
    K key;
    size_t size;
    size_t next_pos;
    explicit OPTNode(K _key, size_t _size, size_t _next_pos) 
      : key(_key), size(_size), next_pos(_next_pos) {}
  };

  struct OPTNodeCompare {
    bool operator()(const OPTNode &a, const OPTNode &b) {
      return a.next_pos > b.next_pos;
    }
  };

  using OPTNodeIter = typename std::multiset<OPTNode, OPTNodeCompare>::iterator;

  void evict() {
    OPTNodeIter node = rbtree_.begin();
    cache_size_ -= chunksize(node->size);
    cache_map_.erase(node->key);
    rbtree_.erase(node);
  }

  using Cache<K>::cache_size_;
  using Cache<K>::cache_capacity_;
  using Cache<K>::chunksize;

  std::unordered_map<K, OPTNodeIter> cache_map_;
  std::multiset<OPTNode, OPTNodeCompare> rbtree_;
};
#endif
