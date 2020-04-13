/**
 * File              : parser.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 15.11.2019
 * Last Modified Date: 15.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#include "file_reader.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <memory>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "util/util.h"

FileReader::operator bool() {
  return !is_->eof();
}

bool FileReader::OpenFile(const std::string &filepath) {
  file_ = std::unique_ptr<std::ifstream>(new std::ifstream(filepath, std::ios_base::in | std::ios_base::binary));
  if (!file_->is_open()) {
    // fail to open file
    return false;
  }
  inbuf_ = std::unique_ptr<IStreamBuf>(new IStreamBuf);
  if (EndWith(filepath, ".gz") || EndWith(filepath, ".GZ")) {
    inbuf_->push(boost::iostreams::gzip_decompressor());
  }
  inbuf_->push(*file_);
  // transfor boost::istreambuf to std::istream
  is_ = std::unique_ptr<std::istream>(new std::istream(inbuf_.get()));
  return true;
}

std::string FileReader::ReadLine() {
  getline(*is_, line_);
  return line_;
}

/* bool FileReader::ReadLine() { */
  // getline(*is_, line_);
  // return !is_->eof();
// }

void FileReader::RewindIs() {
  inbuf_->pop();
  file_->clear();
  file_->seekg(0, std::ios::beg);
  inbuf_->push(*file_);
  // transfor boost::istreambuf to std::istream
  is_ = std::unique_ptr<std::istream>(new std::istream(inbuf_.get()));
}

size_t FileReader::FileLength() {
  size_t res = ::FileLength(*is_);
  RewindIs();
  return res;
}

FileReader &FileReader::operator>> (std::string &word) {
  *is_ >> word;
  return *this;
}

FileReader &FileReader::operator>> (char *word) {
  *is_ >> word;
  return *this;
}

FileReader &FileReader::operator>> (size_t &word) {
  *is_ >> word;
  return *this;
}

FileReader &FileReader::operator>> (double &word) {
  *is_ >> word;
  return *this;
}

