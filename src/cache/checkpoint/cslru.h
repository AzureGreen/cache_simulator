//
// Created by oakzhang on 2020/3/30.
//

#ifndef CHECKPOINT_CSLRU_H
#define CHECKPOINT_CSLRU_H

#include "cache/base/slru.h"
#include <string>
#include "util/util.h"

template <typename T>
class SLRUCheckPoint : public SLRUCache<T> {
 public:
  explicit SLRUCheckPoint(size_t n, size_t capacity)
    : SLRUCache<T>(n, capacity) {}

  ~SLRUCheckPoint() = default;

  bool Push(const std::string &line) {
    auto res = Split(line, '\t');
    if (res.size() == 1) {
      // new segment
      cur_idx_ = std::stoull(res[0]);
    } else {
      T key = static_cast<T>(std::stoull(res[0]));
      size_t size = std::stoull(res[1]);
      size_t chunk_size = this->ChunkSize(size);
      if (this->IsFull(chunk_size)) return false;
      this->lists()[cur_idx_].emplace_back(key, size, cur_idx_);
      this->sizes()[cur_idx_] += chunk_size;
      this->cache_map()[key] = std::prev(this->lists()[cur_idx_].end());
    } 
    return true;
  }
  
  std::string Pop() {
    if (this->Empty()) return "";
    if (cur_idx_ == this->lists().size() - 1 && this->sizes().front() != 0) {
      cur_idx_ = 0;
      return "0";
    } else if (cur_idx_ < this->lists().size() && 
      this->sizes()[cur_idx_] == 0) {
      cur_idx_++;
      return std::to_string(cur_idx_);
    } else {
      auto node = this->lists()[cur_idx_].front();
      this->cache_map().erase(node.key);
      this->lists()[cur_idx_].pop_front();
      size_t chunk_size = this->ChunkSize(node.size);
      this->DecreaseSize(chunk_size);
      this->sizes()[cur_idx_] -= chunk_size;
      char s[260] = {0};
      sprintf(s, "%zu\t%zu", node.key, node.size);
      return s;
    }
  }

 private:
  size_t cur_idx_ = 0;  // used in Push and Pop, idx point to segment
};


#endif //CHECKPOINT_CSLRU_H
