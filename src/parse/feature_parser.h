/**
 * File              : feature_parser.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 29.11.2019
 * Last Modified Date: 29.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef FEATURE_PARSER_H
#define FEATURE_PARSER_H

#include <string>

#include "parser.h"
#include "util/fileio/file_reader.h"


//
// 1.key
// 2.size
// 3.timestamp
// 4.file size
// 5.mtime
// 6.user agent
// 7.file type
//
class FeatureParser {

 public:

  FeatureParser() {}

  ~FeatureParser() {}

  // private data member accessor
  size_t key() const { return key_; }

  size_t size() const { return size_; }

  size_t timestamp() const { return ts_; }

  std::string line() const { return reader_.line(); }

  bool OpenFile(const std::string &file_path) { return reader_.OpenFile(file_path); }

  size_t FileLength() { return reader_.FileLength(); }

  bool ParseLine() {
    std::string &&line = reader_.ReadLine();
    if (reader_.eof()) {
      return false;
    }
    size_t pos = 0;

    ts_ = ParseNumber(line, pos);
    key_ = ParseNumber(line, pos);
    size_ = ParseNumber(line, pos);
    return true;
  }

 private:
  FileReader reader_;

  size_t ts_;
  size_t key_;
  size_t size_;
};


#endif  // UTIL_FEATURE_PARSER_H
