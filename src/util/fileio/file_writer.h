/**
 * File              : file_writer.cc
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 15.11.2019
 * Last Modified Date: 15.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "util/util.h"

class FileWriter {

 public:
  FileWriter() = default

  virtual ~FileWriter() = default

  bool OpenFile(const std::string &filepath);

  virtual void WriteLine(const std::string &line);

  virtual void WriteLine(size_t number);

  virtual void WriteWord(const std::string &word);

  virtual void Flush();

  FileWriter &operator<< (const std::string &word);

  FileWriter &operator<< (const char *word);

  FileWriter &operator<< (size_t word);

  FileWriter &operator<< (int word);

  FileWriter &operator<< (double word);

 private:

  using OStreamBuf = boost::iostreams::filtering_streambuf<boost::iostreams::output>; 

  std::unique_ptr<std::ostream> os_;
  std::unique_ptr<std::ofstream> file_;
  std::unique_ptr<OStreamBuf> outbuf_;
};

#endif  //  FILE_WRITER_H 
