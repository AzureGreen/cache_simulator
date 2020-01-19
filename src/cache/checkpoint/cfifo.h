/**
 * File              : cfifo.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 06.01.2020
 * Last Modified Date: 06.01.2020
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CHECKPOINT_FIFO_H
#define CHECKPOINT_FIFO_H

#include "cache/base/fifo.h"
#include "util/util.h"

template <typename T>
class FIFOCheckPoint : public FIFOCache<T> {
 public:
  explicit FIFOCheckPoint(size_t capacity)
    : FIFOCache<T>(capacity) {}

  ~FIFOCheckPoint() = default;

  /// this function used as load data from checkpoint file to memory.
  /// \param line
  /// \return bool
  bool Push(const std::string &line) {
    auto ret = Split(line, '\t');
    T key = T(std::stoull(ret[0]));
    size_t size = std::stoull(ret[1]);
    size_t chunk_size = this->ChunkSize(size);
    if (this->Full(chunk_size)) return false;
    this->fifo().emplace_back(key, size);
    this->cache_map()[key] = std::prev(this->fifo().end());
    this->IncreaseSize(chunk_size);
    return true;
  }

  /// pop memory data, and transfer as string format
  /// \return
  std::string Pop() {
    if (this->Empty()) return "";
    auto node = this->fifo().front();
    this->cache_map().erase(node.key);
    this->fifo().pop_front();
    this->DecreaseSize(this->ChunkSize(node.size));
    char s[260] = {0};
    sprintf(s, "%zu\t%zu", node.key, node.size);
    return s;
  }
};


#endif // CHECKPOINT_FIFO_H
