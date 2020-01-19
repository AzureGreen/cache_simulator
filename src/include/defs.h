/**
 * File              : defs.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 26.11.2019
 * Last Modified Date: 26.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef INCLUDE_DEF_H
#define INCLUDE_DEF_H

#include <cstddef>
#include <limits>

namespace constant {
  
  const size_t OUT_OF_TRACE = std::numeric_limits<size_t>::max();

  constexpr size_t MAX_FILE_SIZE = 1ull << 40;

  static constexpr size_t KILOBYTE = 1ull << 10;

  static constexpr size_t MEGABYTE = 1ull << 20;

  static constexpr size_t GIGABYTE = 1ull << 30;

  static constexpr size_t TIBIBYTE = 1ull << 30;

  static const double Q_SIZE_RATIO = 0.3;

  static constexpr size_t TIME_UNIT = 60; // 1min

  // 
  // chunk about
  // 
  static constexpr size_t TINY_CHUNK_SIZE = 32 * KILOBYTE;

  static constexpr size_t BIG_CHUNK_SIZE = MEGABYTE;

  // static constexpr size_t BIG_DATA_SIZE = 4 * MEBIBYTE;
};

#endif
