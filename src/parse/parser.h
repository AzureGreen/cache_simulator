/**
 * File              : parser.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 15.11.2019
 * Last Modified Date: 15.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */


#ifndef UTIL_PARSER_H
#define UTIL_PARSER_H

#include <string>
#include <vector>

#include "util/util.h"

std::string ParseString(const std::string &record, size_t &start_pos) {
  size_t next_pos = record.find("\t", start_pos); 
  size_t len = next_pos > 0 ? next_pos - start_pos : std::string::npos;
  std::string ret = record.substr(start_pos, len);
  start_pos = next_pos + 1;
  return ret;
}

size_t ParseNumber(const std::string &record, size_t &start_pos) {
  return std::stoull(ParseString(record, start_pos));
}


std::vector<std::vector<size_t>> ParseRange(const std::string& range, 
    size_t file_size) {
  // skip this case whose file size is zero
  if (file_size == 0) {
    return {{0, 0}};
  }
  size_t begin = 0, end = 0;
  // first of all, split is needed for there may be many a-b,c-d...
  auto ranges = Split(range, ',');
  std::vector<std::vector<size_t>> res;
  for (auto& i : ranges) {
    // get the start and end of this range i
    if (i == "-1") {
      begin = end = file_size - 1;
    } else if (i == "-") {
      // Range:bytes=-
      begin = 0;
      end = file_size - 1;
    } else if (i.front() == '-') {
      // Range:bytes=-500
      // last 500 bytes
      begin = file_size - std::min(file_size, std::stoul(i.substr(1))); 
      end = file_size - 1;
    } else {
      // Range:bytes=500-
      // Range:bytes=start-end
      size_t len = i.find("-");
      begin = std::stoul(i.substr(0, len));
      end = (len == i.size() - 1) ? file_size - 1 : 
        std::min(file_size - 1, std::stoul(i.substr(len + 1)));
    }
    res.push_back({begin, end}); 
  }
  return res;
}


#endif  // UTIL_PARSER_H
