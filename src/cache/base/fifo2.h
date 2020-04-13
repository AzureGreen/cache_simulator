/**
 * File              : fifo2.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 28.08.2019
 * Last Modified Date: 12.08.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */
#ifndef CACHE_FIFO2_H
#define CACHE_FIFO2_H

#include "cache.h"

#include <list>
#include <unordered_map>
#include <cstddef> //size_t

template<class T>
class FIFO2Cache : public Cache<T> {
 public:
  explicit FIFO2Cache(size_t capacity) : Cache<T>(FIFO2, capacity) {}

  ~FIFO2Cache() {
    cache_map_.clear();
    fifo2_.clear();
  }

  bool get(T key, size_t &size) {
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()){
      size = it->second->size;
      it->second->flag = true;
      return true;
    }
    return false;
  }

  bool insert(T key, size_t size) {
    // here, we need loop to find evition
    size_t chunk_size = this->ChunkSize(size);
    while (this->IsFull(chunk_size)) {
      evict();
    }
    fifo2_.emplace_front(key, size);
    cache_map_[key] = fifo2_.begin();
    this->IncreaseSize(chunk_size);
    return true;
  }

 protected:
  struct FIFO2Node {
    T key;
    // size_t size:63; // high 63 bit
    // size_t flag:1;   // low 1 bit
    size_t size;
    bool flag;
    explicit FIFO2Node(T _key, size_t _size): key(_key), size(_size), flag(0) {}
    explicit FIFO2Node(T _key, size_t _size, bool _flag): key(_key), size(_size), flag(_flag) {}
   };

  using FIFO2NodeIter = typename std::list<FIFO2Node>::iterator;

  std::list<FIFO2Node> & fifo2() { return fifo2_; }

  std::unordered_map<T, FIFO2NodeIter> & cache_map() { return cache_map_; }

 private:

  void evict() {
    // here just evict one element
    size_t last_cache_size = this->size();
    while (this->size() == last_cache_size) {
      FIFO2NodeIter node = std::prev(fifo2_.end());
      if (node->flag) {
        // if hit before, put it to the front instead of evict it.
        node->flag = false;
        fifo2_.splice(fifo2_.begin(), fifo2_, node);
      } else {
        this->DecreaseSize(this->ChunkSize(node->size));
        cache_map_.erase(node->key);
        fifo2_.erase(node);
      }
    }
  }

  std::list<FIFO2Node> fifo2_;
  std::unordered_map<T, FIFO2NodeIter> cache_map_;
};
#endif
