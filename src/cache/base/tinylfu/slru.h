/**
 * File              : slru.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 29.08.2019
 * Last Modified Date: 11.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CACHE_TINY_SLRU_H
#define CACHE_TINY_SLRU_H

#include "cache/base/cache.h"

#include <list>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <cstddef>
#include <assert.h>

//
// first -> second -> third -> fourth.
// hit once -> twice -> third -> fourth

namespace tinylfu {
template<class K>
class SLRUCache : public Cache<K> {
 public:
  SLRUCache(size_t capacity)
    : Cache<K>(SLRU, capacity), 
      lists_(2), sizes_(2, 0), max_sizes_(2, 0) {
    max_sizes_[SECOND] = 0.2f * capacity;
    max_sizes_[FIRST] = capacity - max_sizes_[0];    
  }

  ~SLRUCache() {
    cache_map_.clear();
    for (auto &q : lists_) {
      q.clear();
    }
    lists_.clear();
    sizes_.clear();
  }

  K victim_key() const { return lists_[FIRST].back().key; }

  virtual bool get(K key, size_t &size) {
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()) {
      SLRUNodeIter node = it->second;
      size = node->size;

      Move(node->seg, SECOND, node);

      // do some demote
      while (sizes_[SECOND] > max_sizes_[SECOND]) {
        Move(SECOND, FIRST, --lists_[SECOND].end());
      }
      return true;
    }
    return false;
  }

  virtual bool insert(K key, size_t size) {
    size_t chunk_size = chunksize(size);
    while (cache_size() + chunk_size > cache_capacity_) {
      Evict();
    }
    lists_[FIRST].emplace_front(key, size);
    cache_map_[key] = lists_[FIRST].begin();
    sizes_[FIRST] += chunk_size;
    return true;
  }

 private:
  enum Segment { FIRST = 0, SECOND = 1 };
  struct SLRUNode {
    K key;
    size_t size;
    Segment seg;
    explicit SLRUNode(K _key, size_t _size) : key(_key), size(_size), seg(FIRST) {}
  };

  using SLRUNodeIter = typename std::list<SLRUNode>::iterator;

  inline size_t cache_size() const {
    return std::accumulate(sizes_.begin(), sizes_.end(), (size_t)0);
  }

  void Move(Segment from, Segment to, SLRUNodeIter &node) {
    size_t chunk_size = chunksize(node->size);
    node->seg = to;
    sizes_[to] += chunk_size;
    sizes_[from] -= chunk_size;
    lists_[to].splice(lists_[to].begin(), lists_[from], node);
  }

  void Evict() {
    Segment seg = FIRST;
    Segment highest = SECOND;
    while(seg <= highest) {
      if (sizes_[seg] > 0) {
        SLRUNodeIter node = --lists_[seg].end();
        sizes_[seg] -= chunksize(node->size);
        cache_map_.erase(node->key);
        lists_[seg].erase(node);
        break;
      }
      seg = Segment(int(seg) + 1);
    }
  }

  using Cache<K>::cache_capacity_;
  using Cache<K>::cache_size_;
  using Cache<K>::chunksize;

  std::unordered_map<K, SLRUNodeIter> cache_map_;
  std::vector<std::list<SLRUNode>> lists_;
  std::vector<size_t> sizes_;
  std::vector<size_t> max_sizes_;
  
};
};

#endif
