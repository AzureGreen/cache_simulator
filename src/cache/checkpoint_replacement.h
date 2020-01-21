/**
 * File              : checkpoint_replacement.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 14.08.2019
 * Last Modified Date: 19.01.2020
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef CORE_REPLACE_H
#define CORE_REPLACE_H

#include "cache/cache_checkpoint.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <glog/logging.h>
#include "parse/feature_parser.h"
#include "util/util.h"


using CacheKey = size_t;

// test routine 
class CacheTester {

 public:

  CacheTester(algorithm_t algor, size_t cap)
    : instance_(algor, cap) {}

  void RunNormal(FeatureParser &parser) {
    while (parser.ParseLine()) {
      if (!instance_.Get(parser.key(), parser.size())) {
        instance_.Insert(parser.key(), parser.size());
      }
    }
  }

  void Load(const std::string &ckp) {
    if (!ckp.empty()) {
      FileReader reader;
      if (!reader.OpenFile(ckp)) {
        LOG(ERROR) << "Fail to load ckp: " << ckp;
        return;
      }
      instance_.LoadCheckpoint(reader);
      LOG(INFO) << "Succ to load ckp: " << ckp;
    }
  }

  void Save(const std::string &ckp) {
    // check directory exist or not;
    std::string dir_path = FileDirectory(ckp);
    if (!PathExist(dir_path)) {
      // here is safe, no need for file lock
      MakeDir(dir_path);
    }
    FileWriter writer;
    if (!writer.OpenFile(ckp)) {
//      PrintError("Fail to open: " + ckp);
      LOG(ERROR) << "Fail to dump ckp: " << ckp;
      return;
    }
    instance_.SaveCheckpoint(writer);
    LOG(INFO) << "Succ to dump ckp: " << ckp;
  }

  /// start to simulating, firstly load checkpoint if necessary,
  /// secondly, run all files, finally, save checkpoint.
  /// \param chunk_files paths of all trace files
  /// \param prev_cpf yesterday's checkpoint file path
  /// \param cur_cpf  today's checkpoint file path
  /// \param hit_bytes  today's hit bytes
  /// \param request_bytes  today's request bytes
  void Simulate(const std::vector<std::string> &chunk_files,
      const std::string &prev_cpf, const std::string &cur_cpf,
      size_t &hit_bytes, size_t &request_bytes) {

    Load(prev_cpf);
    for (auto &chunk_file: chunk_files) {
      FeatureParser parser;
      if (!parser.OpenFile(chunk_file)) {
        LOG(ERROR) << "Fail to parse file: " << chunk_file;
        PrintError("Fail to open file: " + chunk_file);
      }
      RunNormal(parser);
    }
    Save(cur_cpf);
    hit_bytes = instance_.FluxHit();
    request_bytes = instance_.FluxOut();
  }
 private :
  CacheCheckpoint<CacheKey> instance_;
};

#endif


