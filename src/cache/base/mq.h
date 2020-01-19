/**
 * File              : mq.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 23.08.2019
 * Last Modified Date: 11.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef BASE_MQ_H
#define BASE_MQ_H

#include "cache.h"

#include <list>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <cstddef> //size_t

template<class K>
class MQCache : public Cache<K> {
 public:
  MQCache(int num, size_t life_time, size_t capacity)
    : Cache<K>(LRU, capacity),
      queues_(num), 
      out_capacity_(capacity),
      out_size_(0),
      current_time_(0),
      life_time_(life_time) {}

  ~MQCache() {
    for (auto& q : queues_) {
      q.clear();
    }
    queues_.clear();
    qout_.clear();
    cache_map_.clear();
  }

  bool get(K key, size_t &size) {
    it_ = cache_map_.find(key);
    if (it_ != cache_map_.end() && it_->second->tag != -1) {
      MQNodeIter node_iter = it_->second;
      size = node_iter->size;
      // reinsert
      Reinsert(node_iter, queues_[node_iter->tag]);
      Adjust();
      return true;
    }
    return false;
  }

  bool insert(K key, size_t size) {
    size_t chunk_size = chunksize(size);
    if (it_ != cache_map_.end() && it_->second->tag == -1) {
      // there may be a limit situation, hit last element in Q_out, but queue is
      // full at the same time, so, we need skip the last element in Q_out, to
      // evict second last element in Q_out, so, just move last element before
      // evict.
      MQNodeIter node_it = it_->second;
      Reinsert(node_it, qout_);
      out_size_ -= chunk_size;
    } else {
      // insert new node
      queues_[0].emplace_front(key, size, current_time_ + life_time_);
      cache_map_[key] = queues_[0].begin();
    }
    // evict (demote q and evict Q_out)
    while (cache_size_ + chunk_size > cache_capacity_) {
      evict();
    }
    Adjust();
    cache_size_ += chunk_size;
    return true;
  }

 private:
  struct MQNode {
    K key;
    size_t size;
    size_t freq;
    int    tag;   // which queue
    size_t expire_time;
    explicit MQNode(K _key, size_t _size, size_t _expire_time) 
      : key(_key), size(_size), freq(1), tag(0), 
        expire_time(_expire_time) {}
  };

  using MQNodeIter = typename std::list<MQNode>::iterator;
  
  void evict() {
    int i = 0;
    // find fisrt none empty lru queue
    while (queues_[i].empty()) { i++; }

    Evict2QOut(queues_[i]);
    // delete lru
  }

  void Evict2QOut(std::list<MQNode>& q) {
    MQNodeIter node = --q.end();
    size_t chunk_size = chunksize(node->size);
    // cycle evict out queue to make room 
    while (out_size_ + chunk_size >= out_capacity_) {  
      // evict Q_out node
      MQNodeIter out_node = --qout_.end();
      out_size_ -= chunksize(out_node->size);
      cache_map_.erase(out_node->key);
      qout_.erase(out_node);
    }
    qout_.splice(qout_.begin(), q, node);
    node->tag = -1;
    out_size_ += chunk_size;
    cache_size_ -= chunk_size;
  }

  void Reinsert(MQNodeIter &node, std::list<MQNode> &from) { 
    node->freq++;
    int idx = QueueNum(node->freq);
    queues_[idx].splice(queues_[idx].begin(), from, node);
    node->tag = idx;
    node->expire_time = current_time_ + life_time_;
  }


  // demote frequently accessed blocks which haven't been visited recently.
  void Adjust() {
    current_time_++;
    for (size_t k = 1; k < queues_.size(); k++) {
      // skip empty lru queue
      if (queues_[k].empty()) {
        continue;
      }
      auto node = --queues_[k].end();
      if (node->expire_time < current_time_) {
        queues_[k - 1].splice(queues_[k - 1].begin(), queues_[k], node);
        node->tag = k - 1;
        node->expire_time = current_time_ + life_time_;
      }
    }
  }

  inline int QueueNum(size_t freq) {
    return std::min((size_t)std::floor(std::log2(freq)), queues_.size() - 1);
  }

  using Cache<K>::cache_size_;
  using Cache<K>::cache_capacity_;
  using Cache<K>::chunksize;

  typename std::unordered_map<K, MQNodeIter>::iterator it_; 
  std::unordered_map<K, MQNodeIter> cache_map_;
  std::vector<std::list<MQNode>> queues_;
  std::vector<size_t> size_;
  std::list<MQNode> qout_;
  size_t out_capacity_;   // the capacity of Q_out
  size_t out_size_;   // the size of Q_out
  size_t current_time_;
  size_t life_time_; 
};


#endif
