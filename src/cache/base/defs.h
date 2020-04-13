/**
 * File              : defs.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 03.12.2019
 * Last Modified Date: 03.12.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CACHE_DEF_H
#define CACHE_DEF_H

#include <string>

#include "include/defs.h"

typedef enum {
  // traditional
  LRU = 0,
  ARC,
  SLRU,
  S3LRU,
  S4LRU,
  S2LRU,
  LIRS,
  OPT,
  FIFO,
  FIFO2,
  CLOCK,
  // LFU,
  // GDSF,
  MQ,
  TQ,
  TINYLFU,
  LARC,
  UNKNOWN_ALGOR
} algorithm_t;


static inline
algorithm_t ReplacementAlgorithm(const std::string &name){
  std::string upper_name; 
  for(char c : name) {
    upper_name.push_back(std::toupper(c));
  }
  if (upper_name == "ARC"){
    return ARC; 
  } else if (upper_name == "LIRS"){
    return LIRS;
  } else if (upper_name == "LRU"){
    return LRU;
  } else if (upper_name == "S3LRU"){
    return S3LRU;
  } else if (upper_name == "S4LRU"){
    return S4LRU;
  } else if (upper_name == "OPT"){
    return OPT;
  } else if (upper_name == "FIFO"){
    return FIFO;
  } else if (upper_name == "FIFO2"){
    return FIFO2;
  // } else if (upper_name == "CLOCK"){
    // return CLOCK;
  // } else if (upper_name == "LFU"){
    // return LFU;
  }else if (upper_name == "SLRU") {
    return SLRU;
  // } else if (upper_name == "GDSF") {
    // return GDSF; 
  } else if (upper_name == "MQ") {
    return MQ; 
  } else if (upper_name == "2Q") {
    return TQ; 
  } else if (upper_name == "TINYLFU") {
    return TINYLFU;
  } else if (upper_name == "LARC") {
    return LARC;
  }
  return UNKNOWN_ALGOR;
}

static inline
std::string AlgorithmName(algorithm_t algor) {
  switch (algor) {
    case LRU:
      return "LRU";
    case FIFO:
      return "FIFO";
    case S3LRU:
      return "S3LRU";
    case LIRS:
      return "LIRS";
    case ARC:
      return "ARC";
    case FIFO2:
      return "FIFO2";
    case MQ:
      return "MQ";
    case TQ:
      return "2Q";
    case TINYLFU:
      return "TINYLFU";
    case LARC:
      return "LARC";
    default:
      return "UNKNOWN_ALGOR";
  }
}

//static inline
//size_t ChunkSize(size_t size) {
//  return size == constant::BIG_CHUNK_SIZE ? constant::BIG_CHUNK_SIZE :
//    constant::TINY_CHUNK_SIZE;
//}


#endif  

