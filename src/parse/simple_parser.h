/**
 * File              : simple_parser.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 15.11.2019
 * Last Modified Date: 15.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef SIMPLE_PARSER_H
#define SIMPLE_PARSER_H

#include "include/defs.h"
#include "parser.h"
#include "util/fileio/file_reader.h"


//
// 1.timestamp
// 2.method
// 3.host
// 4.url
// 5.urlparam
// 6.file_size
// 7.ranges
// 8.mtime
// 9.user_agent
// 10.file_type

class SimpleParser {

 public:

  // when it is sampling, just use default false
  // but when using sampled data, sample_version should be true
  SimpleParser(bool _sample_version=false) 
    : add_param_(false), sample_version_(_sample_version) {}
  ~SimpleParser() {}

  // private data member accessor
  size_t timestamp() const { return ts_; }
  size_t mtime() const { return mtime_; }
  size_t file_size() const { return file_size_; }
  size_t user_agent() const { return user_agent_; }
  const std::string host() const { return host_; }
  const std::string url() const { return url_; }
  const std::string urlparam() const { return urlparam_; }
  const std::string method() const { return method_; }
  const std::string ranges() const { return ranges_; }
  const std::string file_type() const { return file_type_; }
  // const std::string file_name() const { return method_ == "POST" ? host_ + url_ : host_ + url_ + "?" + urlparam_; }
  const std::string line() const { return reader_.line(); }
  const std::string file_name() const { 
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
    method_ = ParseString(line, pos);
    host_ = ParseString(line, pos);
    url_ = ParseString(line, pos);
    urlparam_ = ParseString(line, pos);
    file_size_ = ParseNumber(line, pos);
    ranges_ = ParseString(line, pos);
    mtime_ = ParseNumber(line, pos);
    user_agent_ = ParseNumber(line, pos);
    file_type_ = ParseString(line, pos);
    if (sample_version_) add_param_ = (bool)ParseNumber(line, pos);
    
    // handle some error for file size
    if (file_size_ > constant::MAX_FILE_SIZE) {
      file_size_ = 0 ;
    }

    return true;
  }  


 private:
  FileReader reader_;

  bool add_param_;
  bool sample_version_;
  size_t ts_;
  size_t mtime_;
  size_t file_size_;
  size_t user_agent_;
  // size_t file_type_;
  std::string method_;
  std::string host_;
  std::string url_;
  std::string urlparam_;
  std::string ranges_;
  std::string file_type_;
};



#endif  // UTIL_SIMPLE_PARSER_H
