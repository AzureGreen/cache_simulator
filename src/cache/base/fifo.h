/**
 * File              : fifo.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 03.12.2019
 * Last Modified Date: 03.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */
#ifndef CACHE_FIFO_H
#define CACHE_FIFO_H

#include "cache.h"

#include <list>
#include <unordered_map>
#include <cstddef> //size_t
#include <queue>

template <typename T>
class FIFOCache : public Cache<T> {
 public:
  struct FIFONode {
    T      key;
    size_t size;
    explicit FIFONode(T _key, size_t _size) : key(_key), size(_size) {}
  };

  using FIFONodeIter = typename std::list<FIFONode>::iterator;

  FIFOCache(size_t capacity) : Cache<T>(FIFO, capacity) {}

  ~FIFOCache() {
    cache_map_.clear();
    fifo_.clear();
  }

  inline std::list<struct FIFONode> & fifo() { return fifo_; }

  inline std::unordered_map<T, typename std::list<struct FIFONode>::iterator>
      & cache_map() { return cache_map_; }

  virtual bool get(T key, size_t &size) {
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()){
      size = it->second->size;
      return true;
    }
    return false;
  }

  virtual bool insert(T key, size_t size) {
    size_t chunk_size = this->ChunkSize(size);
    while (this->Full(chunk_size)) {
      evict();
    }
    fifo_.emplace_front(key, size);
    cache_map_[key] = fifo_.begin();
    this->IncreaseSize(chunk_size);
    return true;
  }

 private:

  void evict() {
    FIFONodeIter node = std::prev(fifo_.end());
    this->DecreaseSize(this->ChunkSize(node->size));
    cache_map_.erase(node->key);
    fifo_.erase(node);
  }

  std::list<FIFONode> fifo_;
  std::unordered_map<T, FIFONodeIter> cache_map_;
};
#endif
