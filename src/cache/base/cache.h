/**
 * File              : cache.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 29.08.2019
 * Last Modified Date: 03.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */
#ifndef CACHE_H
#define CACHE_H

#include <cstddef>

#include "defs.h"
#include "include/defs.h"

template <typename T>
class Cache {
 public:
  Cache(algorithm_t name, size_t capacity)
    : name_(name), cache_size_(0), cache_capacity_(capacity) {}

  virtual ~Cache() {}

  virtual algorithm_t name() const { return name_; }

  virtual size_t capacity() const { return cache_capacity_; }

  virtual size_t size() const { return cache_size_; }

  virtual bool Empty() const { return cache_size_ == 0; }

  virtual bool IsFull(size_t chunk_size) const { return cache_size_ + chunk_size > cache_capacity_; }

//  virtual size_t chunksize(size_t size) const { return ChunkSize(size); }

  virtual void IncreaseSize(size_t size) { cache_size_ += size; }

  virtual void DecreaseSize(size_t size) { cache_size_ -= size; }

  virtual size_t ChunkSize(size_t size) {
    return size == constant::BIG_CHUNK_SIZE ? constant::BIG_CHUNK_SIZE :
           constant::TINY_CHUNK_SIZE;
  }

  virtual bool Push(const std::string &line) { return false; }

  virtual std::string Pop() { return ""; }

  virtual bool get(T key, size_t &size) {
    (void) key;
    (void) size;
    return false;
  }

  virtual bool get(T key, size_t &size, size_t data) {
    (void) data;
    return get(key, size);
  }

  virtual bool insert(T key, size_t size) {
    (void) key;
    (void) size;
    return true;
  }

  virtual bool insert(T key, size_t size, size_t data) {
    (void) data;
    return insert(key, size);
  }

  virtual bool admit(T) { return true; }

  virtual void clear() {}

 private:
  algorithm_t name_;
  size_t cache_size_;    
  size_t cache_capacity_;
};

#endif
