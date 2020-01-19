/**
 * File              : arc.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 29.08.2019
 * Last Modified Date: 10.10.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */
#ifndef CACHE_ARC_H
#define CACHE_ARC_H

#include "cache.h"

#include <list>
#include <unordered_map>
#include <cstddef>
#include <algorithm>

template<class K>
class ARCCache : public Cache<K> {
 public:

  ARCCache(size_t capacity)
    : Cache<K>(ARC, capacity), 
      lists_(4), sizes_(4), p_(0u) {}

  ~ARCCache() {
    cache_map_.clear();
    for (auto &l : lists_) {
      l.clear();
    }
    lists_.clear();
    sizes_.clear();
  }


  bool get(K key, size_t &size) {
    it_ = cache_map_.find(key);
    if (it_ != cache_map_.end()){
      // hit in T1/T2/B1/B2
      ARCNodeIter node_iter = it_->second;
      ARCNode &node = *node_iter;
      size = node.size;
      switch (node.flag) {
        case T1:  // Case I
          Promote(T1, T2, node_iter);
          return true;
        case T2:  // Case I
          Promote(T2, T2, node_iter);
          return true;
        default:
          return false;
      }
    } else {
      return false;
    }
  }

  bool insert(K key, size_t size) { 
    size_t theta = 1;
    // size_t chunk_size = chunksize(size);
    size_t chunk_size = constant::TINY_CHUNK_SIZE;
    // hit in B1 or B2
    if (it_ != cache_map_.end()) {
      ARCNodeIter node = it_->second;
      switch(node->flag) {
        case B1:    // Case II
          // update p
          theta = std::max(1, int(sizes_[B2] / sizes_[B1]));
          p_ = std::min(cache_capacity_, p_ + theta * chunk_size);

          Replace(node->flag);
          Promote(B1, T2, node);
          break;
        case B2:    // Case III
          // update p
          theta = std::max(1, int(sizes_[B1] / sizes_[B2]));
          p_ = std::max(size_t(0), p_ - theta * chunk_size);

          Replace(node->flag);
          Promote(B2, T2, node);
          break;
        default:
          break;
      }
    } else { 
      // miss Case IV 

      //L1 has exactly c pages  ( Case A )
      if (l1_size() >= cache_capacity_) {
        // T1 remain space
        if (sizes_[T1] < cache_capacity_) {
          Evict(B1);
          Replace(T1);
        } else { 
          // B1 is empty
          Evict(T1);
        }
      } else {
        // L1 has less than c pages  ( Case B ) 
        if (total_size() >= cache_capacity_) {
          if (total_size() >= 2 * cache_capacity_) {
            Evict(B2);
          }
          Replace(T1);
        }
      }

      // insert new node
      lists_[T1].emplace_front(key, size); 
      cache_map_[key] = lists_[T1].begin();
      sizes_[T1] += chunksize(size);
      // sizes_[T1] += chunk_size;
    }
    return true;
  }

 private:
  enum ARCQueue { T1 = 0, T2 = 1, B1 = 2, B2 = 3 };
  struct ARCNode {
    K key;
    size_t size;
    ARCQueue flag;
    explicit ARCNode(K _key, size_t _size) 
      : key(_key), size(_size), flag(T1) {}
  };

  using ARCNodeIter = typename std::list<ARCNode>::iterator;    

  inline size_t cache_size() const {
    return sizes_[T1] + sizes_[T2];
  }

  inline size_t ghost_size() const {
    return sizes_[B1] + sizes_[B2];
  }

  inline size_t l1_size() const {
    return sizes_[T1] + sizes_[B1];
  }

  inline size_t total_size() const {
    return cache_size() + ghost_size();
  }

  void Replace(ARCQueue flag) {

    bool flag1 = false;
    bool flag2 = false;
    bool flag3 = false;
    size_t threshold = std::max(p_, constant::TINY_CHUNK_SIZE); 

    if (sizes_[T1] > 0) {
      flag1 = true;
    }
    if ((flag == B2) && (sizes_[T1] >= threshold)) {
      flag2 = true;
    }
    if (sizes_[T1] > threshold) {
      flag3 = true;
    }
    if (flag1 && (flag2 || flag3)) {
      // demote t1's lru to b1's mru | frequency
      Demote(T1, B1);
    } else {
      // demote t2's lru to b2's mru | recency
      Demote(T2, B2);
    }
  }

  void Demote(ARCQueue from, ARCQueue to) {
    ARCNodeIter node = --lists_[from].end();
    Move(from, to, node);
  }

  void Promote(ARCQueue from, ARCQueue to, ARCNodeIter &node) {
    Move(from, to, node);
  }

  inline void Move(ARCQueue from, ARCQueue to, ARCNodeIter &node) {
    size_t chunk_size = chunksize(node->size);
    lists_[to].splice(lists_[to].begin(), lists_[from], node);
    node->flag = to;
    sizes_[from] -= chunk_size;
    sizes_[to] += chunk_size;
  }

  void Evict(ARCQueue flag) {
    ARCNodeIter node = --lists_[flag].end();
    sizes_[flag] -= chunksize(node->size);
    cache_map_.erase(node->key);
    lists_[flag].erase(node);
  }

  using Cache<K>::cache_capacity_;
  using Cache<K>::cache_size_;
  using Cache<K>::chunksize;

  typename std::unordered_map<K, ARCNodeIter>::iterator it_; 
  std::unordered_map<K, ARCNodeIter> cache_map_; // map of T1, T2, B1, B2
  std::vector<std::list<ARCNode>> lists_; // lists of T1, T2, B1, B2 
  std::vector<size_t> sizes_; // size of T1 B1 T2 B2 
  size_t p_; // upper bound of L1
};

#endif
