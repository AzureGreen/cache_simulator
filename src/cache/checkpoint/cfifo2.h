/**
 * File              : cfifo2.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 31.03.2020
 * Last Modified Date: 31.03.2020
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CHECKPOINT_FIFO2_H
#define CHECKPOINT_FIFO2_H

#include "cache/base/fifo2.h"
#include <string>
#include "util/util.h"

template <typename T>
class FIFO2CheckPoint : public FIFO2Cache<T> {
 public:
  explicit FIFO2CheckPoint(size_t capacity)
    : FIFO2Cache<T>(capacity) {}

  ~FIFO2CheckPoint() = default;

  /// this function used as load data from checkpoint file to memory.
  /// \param line
  /// \return bool
  bool Push(const std::string &line) {
    auto ret = Split(line, '\t');
    T key = static_cast<T>(std::stoull(ret[0]));
    size_t size = std::stoull(ret[1]);
    bool flag = static_cast<bool>(std::stoi(ret[2]));
    size_t chunk_size = this->ChunkSize(size);
    if (this->IsFull(chunk_size)) return false;
    this->fifo2().emplace_back(key, size, flag);
    this->cache_map()[key] = std::prev(this->fifo2().end());
    this->IncreaseSize(chunk_size);
    return true;
  }

  /// pop memory data, and transfer as string format
  /// \return
  std::string Pop() {
    if (this->Empty()) return "";
    auto node = this->fifo2().front();
    this->cache_map().erase(node.key);
    this->fifo2().pop_front();
    this->DecreaseSize(this->ChunkSize(node.size));
    char s[260] = {0};
    sprintf(s, "%zu\t%zu\t%d", node.key, node.size, node.flag);
    return s;
  }
};

#endif // CHECKPOINT_LRU_H
