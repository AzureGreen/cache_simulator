/**
 * File              : parser.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 15.11.2019
 * Last Modified Date: 15.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef FILE_READER_H
#define FILE_READER_H

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "util/util.h"

class FileReader {

 public:
  FileReader() {}

  virtual ~FileReader();

  std::string line() const { return line_; }
  
  std::string &l_line() { return line_; }
  
  std::istream &ios() const { return *is_; }

  bool eof() const { return is_->eof(); }

  bool fail() const { return is_->fail(); }

  bool OpenFile(const std::string &filepath);

  virtual std::string ReadLine();

  void RewindIs();

  size_t FileLength();

  operator bool();

  FileReader &operator>> (std::string &word);

  FileReader &operator>> (char *word);

  FileReader &operator>> (size_t &word);

 private:

  using IStreamBuf = boost::iostreams::filtering_streambuf<boost::iostreams::input>; 

  std::unique_ptr<std::istream> is_;
  std::unique_ptr<std::ifstream> file_;
  std::unique_ptr<IStreamBuf> inbuf_;
  std::string line_;

};


#endif  // UTIL_PARSER_H
