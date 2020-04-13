/**
 * File              : file_writer.cc
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 15.11.2019
 * Last Modified Date: 15.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */


#include "file_writer.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <memory>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "util/util.h"

bool FileWriter::OpenFile(const std::string &filepath) {
  file_ = std::unique_ptr<std::ofstream>(new std::ofstream(filepath, std::ios_base::out | std::ios_base::binary));
  if (!file_->is_open()) {
    // fail to open file
    return false;
  }
  outbuf_ = std::unique_ptr<OStreamBuf>(new OStreamBuf);
  if (EndWith(filepath, ".gz") || EndWith(filepath, ".GZ")) {
    outbuf_->push(boost::iostreams::gzip_compressor());
  }
  outbuf_->push(*file_);
  // transfor boost::istreambuf to std::istream
  os_ = std::unique_ptr<std::ostream>(new std::ostream(outbuf_.get()));
  return true;
}

void FileWriter::WriteLine(const std::string &line) {
  *os_ << line << std::endl;
}

void FileWriter::WriteLine(size_t number) {
  *os_ << number << std::endl;
}

void FileWriter::WriteWord(const std::string &word) {
  *os_ << word;
}


void FileWriter::Flush() {
  // here, flush is different from the normal ostream,
  // here pop operation is needed.
  os_->flush();
  outbuf_->pop();
  file_->flush();
  outbuf_->push(*file_);
  os_ = std::unique_ptr<std::ostream>(new std::ostream(outbuf_.get()));
}


FileWriter &FileWriter::operator<< (const std::string &word) {
  *os_ << word;
  return *this;
}

FileWriter &FileWriter::operator<< (const char *word) {
  *os_ << word;
  return *this;
}

FileWriter &FileWriter::operator<< (size_t word) {
  *os_ << word;
  return *this;
}

FileWriter &FileWriter::operator<< (int word) {
  *os_ << word;
  return *this;
}

FileWriter &FileWriter::operator<< (double word) {
  *os_ << word;
  return *this;
}
