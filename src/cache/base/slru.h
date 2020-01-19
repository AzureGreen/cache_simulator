/**
 * File              : slru.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 29.08.2019
 * Last Modified Date: 11.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */
#ifndef CACHE_SLRU_H
#define CACHE_SLRU_H

#include "cache.h"

#include <list>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <cstddef>
#include <assert.h>

//
// first -> second -> third -> fourth.
// hit once -> twice -> third -> fourth

template<class K>
class SLRUCache : public Cache<K> {
 public:
  SLRUCache(size_t n, size_t capacity)
    : Cache<K>(SLRU, capacity), 
      lists_(n), sizes_(n, 0), avg_capacity_(cache_capacity_ / n) {}

  ~SLRUCache() {
    cache_map_.clear();
    for (auto &q : lists_) {
      q.clear();
    }
    lists_.clear();
    sizes_.clear();
  }

  virtual bool get(K key, size_t &size) {
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()) {
      SLRUNodeIter node_iter = it->second;
      size = node_iter->size;

      Promote(node_iter);

      // do some demote
      Segment seg = HighestLevel();
      while (seg > 0) {
        while (sizes_[seg] > avg_capacity_) {
          Demote(seg);
        }
        seg = Segment(int(seg) - 1);
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
  enum Segment { FIRST = 0, SECOND = 1, THIRD = 2, FOURTH = 3 };
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

  inline Segment HighLevel(Segment low) {
    return (Segment)std::min((int)FOURTH, std::min(low + 1, (int)lists_.size() - 1));
  }

  inline Segment LowLevel(Segment high) {
    return (Segment)std::max((int)FIRST, high - 1);
  }

  inline Segment HighestLevel() {
    return (Segment)std::min((int)FOURTH, (int)lists_.size() - 1);
  }

  void Promote(SLRUNodeIter &node) {
    Segment from = node->seg;
    Segment to = HighLevel(from);
    Move(from, to, node);
  }

  void Demote(Segment seg) {
    SLRUNodeIter node = --lists_[seg].end();
    Segment low = LowLevel(seg);
    Move(seg, low, node);
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
    Segment highest = HighestLevel();
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
  size_t avg_capacity_;  // average capacity of each lru list
  
};

#endif
