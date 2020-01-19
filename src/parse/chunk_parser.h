/**
 * File              : chunkparser.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 27.11.2019
 * Last Modified Date: 27.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef UTIL_CHUNKPARSER_H
#define UTIL_CHUNKPARSER_H

#include <vector>
#include <algorithm>

#include "include/defs.h"
#include "parser.h"
#include "util/fileio/file_reader.h"
#include "util/hash/murmurhash.h"

class ChunkParser {
 public:
  struct Chunk {
    size_t real_size;
    size_t key;
    explicit Chunk(size_t _real_size, size_t _key)
      : real_size(_real_size), key(_key) {}
  };

  ChunkParser() {}
  ~ChunkParser() = default;

  // private data member accessor
  size_t timestamp() const { return ts_; }
  size_t mtime() const { return mtime_; }
  size_t file_size() const { return file_size_; }
  int user_agent() const { return user_agent_; }
  std::string host() const { return host_; }
  std::string url() const { return url_; }
  std::string urlparam() const { return urlparam_; }
  std::string method() const { return method_; }
  std::string ranges() const { return ranges_; }
  std::string file_type() const { return file_type_; }
  std::string line() const { return reader_.line(); }
  std::vector<Chunk> chunks() const { return chunks_; }

  std::string file_name() const {
    // return (method_ == "POST" || urlparam_ == "-") ? host_ + url_ : host_ + url_ + "?" + urlparam_; 
    if (add_param_) {
      return host_ + url_ + "?" + urlparam_;
    } else {
      return host_ + url_;
    }
  }

  bool OpenFile(const std::string &file_path) { return reader_.OpenFile(file_path); }

  size_t FileLength() { return reader_.FileLength(); }

  bool ParseLine() {
    std::string &&line = reader_.ReadLine();            
    if (reader_.eof()) {
      return false;
    }
    size_t pos = 0;

    ts_ = ParseNumber(line, pos);
    host_ = ParseString(line, pos);
    url_ = ParseString(line, pos);
    urlparam_ = ParseString(line, pos);
    file_size_ = ParseNumber(line, pos);
    ranges_ = ParseString(line, pos);
    mtime_ = ParseNumber(line, pos);
    user_agent_ = (int)ParseNumber(line, pos);
    file_type_ = ParseString(line, pos);
    add_param_ = (bool)ParseNumber(line, pos);

    // handle some error for file size
    if (file_size_ > constant::MAX_FILE_SIZE) {
      file_size_ = 0 ;
    }

    // extra parse for range
    auto ranges = ParseRange(ranges_, file_size_);  // split ranges
    SliceChunk(ranges);
    
    return true;
  }  

 private:
  
  void SliceChunk(std::vector<std::vector<size_t>> &ranges) {
    chunks_.clear();
    char suffix[64] = {0};
    size_t size = 0;
    for (auto &range : ranges) {
      size_t big_begin = range[0] / constant::BIG_CHUNK_SIZE;
      size_t big_end = range[1] / constant::BIG_CHUNK_SIZE + 1;     // important for plus 1
      size_t big_last_begin = file_size_ / constant::BIG_CHUNK_SIZE;  // begin of the last part of file size
      size_t tiny_begin = std::max(constant::BIG_CHUNK_SIZE * big_last_begin, range[0]) / constant::TINY_CHUNK_SIZE;
      size_t tiny_end = std::min(range[1], file_size_) / constant::TINY_CHUNK_SIZE + 1;
      size_t tiny_last_begin = file_size_ / constant::TINY_CHUNK_SIZE;
      // first use big chunk size to slice
      while (big_begin < big_last_begin && big_begin < big_end) {
        snprintf(suffix, sizeof(suffix), "_chunk%02lu", big_begin);
        std::string chunk_name = file_name() + suffix;
        size = constant::BIG_CHUNK_SIZE;
        size_t key = MurmurHash64A(chunk_name.data(), chunk_name.size(), 0);
        chunks_.emplace_back(size, key);
        big_begin++;
        // if (big_begin >= big_last_begin) break;
      }
      // second, use tiny chunk size to slice the left
      while (tiny_begin < tiny_end) {
        snprintf(suffix, sizeof(suffix), "_chunk%02lu", big_begin);
        std::string chunk_name = file_name() + suffix;
        if (tiny_begin < tiny_last_begin) {
          size = constant::TINY_CHUNK_SIZE;
        } else {
          size = file_size_ - tiny_begin * constant::TINY_CHUNK_SIZE;
        }
        size_t key = MurmurHash64A(chunk_name.data(), chunk_name.size(), 0);
        chunks_.emplace_back(size, key);
        tiny_begin++;
        big_begin++;
      }
    }
  }

  FileReader reader_;

  size_t ts_{};
  size_t mtime_{};
  size_t file_size_{};
  int user_agent_{};
  // size_t file_type_;
  std::string method_;
  std::string host_;
  std::string url_;
  std::string urlparam_;
  std::string ranges_;
  std::string file_type_;
  bool add_param_{};

  std::vector<Chunk> chunks_;
};

#endif 

