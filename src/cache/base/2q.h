/**
 * File              : 2q.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 09.12.2019
 * Last Modified Date: 09.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CACHE_BASE_2Q_H
#define CACHE_BASE_2Q_H

#include "cache/base/cache.h"

#include <list>
#include <vector>
#include <unordered_map>
#include <cstddef> //size_t

template<class K>
class TQCache : public Cache<K> {
 public:
  TQCache(size_t capacity)
    : Cache<K>(TQ, capacity), sizes_(3), 
      max_sizes_({size_t(capacity * 0.95), capacity - size_t(capacity * 0.95), 
          size_t(capacity)}) {}

  ~TQCache() {
    Am_.clear();
    A1in_.clear();
    A1out_.clear();
    cache_map_.clear();
  }

  bool get(K key, size_t &size) {
    it_ = cache_map_.find(key);
    if (it_ != cache_map_.end()) {
      switch (it_->second->q) {
        case AM:
          Am_.splice(Am_.begin(), Am_, it_->second);
          size = it_->second->size;
          return true;
        case A1IN:
          // do nothing
          size = it_->second->size;
          return true;
        default:
          return false;
      }
    }
    return false;
  }

  bool insert(K key, size_t size) {
    size_t chunk_size = chunksize(size);
    while (cache_size() + chunk_size > cache_capacity_) {
      reclaimfor();
    }
    // after doing evict from A1out, maybe it need to be updated...
    // because the item need to be inserted has just been evicted just now.
    if (it_ != cache_map_.end() && it_->second->q == A1OUT) {
      // here, find again, avoiding evict before reinsertion
      it_ = cache_map_.find(key);
      if (it_ != cache_map_.end()) {
        it_->second->q = AM;
        Am_.splice(Am_.begin(), A1out_, it_->second);
        sizes_[A1OUT] -= chunk_size;
      } else {
        Am_.emplace_front(key, size);
        cache_map_[key] = Am_.begin();
      }
      sizes_[AM] += chunk_size;
    } else {
      A1in_.emplace_front(key, size);
      cache_map_[key] = A1in_.begin();
      sizes_[A1IN] += chunk_size;
    }
    return true;
  }

 private:
  enum QueueType { AM = 0, A1IN, A1OUT };
  struct TQNode {
    K key;
    size_t size;
    QueueType q;
    explicit TQNode(K _key, size_t _size) : key(_key), size(_size), q(A1IN) {}
  };

  using TQNodeIter = typename std::list<TQNode>::iterator;

  inline size_t cache_size() const {
    return sizes_[AM] + sizes_[A1IN];
  }

  void reclaimfor() {
    if (sizes_[A1IN] >= max_sizes_[A1IN]) {
      // check a1out full or not
      auto node = --A1in_.end();
      size_t chunk_size = chunksize(node->size);
      // cycle evict a1out queue to make room for a1in tail node
      while (sizes_[A1OUT] + chunk_size >= max_sizes_[A1OUT]) {
        auto out_node = --A1out_.end();
        sizes_[A1OUT] -= chunksize(out_node->size);
        cache_map_.erase(out_node->key);
        A1out_.erase(out_node);
      }
      node->q = A1OUT;
      A1out_.splice(A1out_.begin(), A1in_, node);
      sizes_[A1IN] -= chunk_size;
      sizes_[A1OUT] += chunk_size;
    } else {
      auto it = --Am_.end();
      cache_map_.erase(it->key);
      sizes_[AM] -= chunksize(it->size);
      Am_.erase(it);
    }
  }

  using Cache<K>::cache_size_;
  using Cache<K>::cache_capacity_;
  using Cache<K>::chunksize;

  typename std::unordered_map<K, TQNodeIter>::iterator it_; 
  std::unordered_map<K, TQNodeIter> cache_map_;
  std::list<TQNode> Am_;
  std::list<TQNode> A1in_;
  std::list<TQNode> A1out_; 
  std::vector<size_t> sizes_;
  std::vector<size_t> max_sizes_;
};

#endif 

