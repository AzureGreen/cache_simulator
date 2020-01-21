/**
 * File              : filemgr.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 25.11.2019
 * Last Modified Date: 25.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef UTIL_FILEMGR_H
#define UTIL_FILEMGR_H

#include <assert.h>
#include <string>

#include "util/singleton.h"
#include "util/util.h"
#include "cache/base/defs.h"
#include "include/defs.h"

class FileMgr : public Singleton<FileMgr> {
 public:
  void Initialize(const std::string &ip, const std::string &dt,
      int bucket_id, const std::string &algor_name, std::string &size) {
    ip_ = ip;
    dt_ = dt;
    bucket_id_ = std::to_string(bucket_id);
    algor_name_ = algor_name;
    size_ = size;
    base_dir_ = "/data/cache_simulator";
  }

  std::string TraceDir() const {
    return PathJoin(base_dir_, ip_, dt_, bucket_id_, "logs");
  }

  std::string LogDir() const {
    return PathJoin(base_dir_, ip_, dt_, "logs", "cache");
  }

  std::string PrevCheckpoint() const {
    auto prev_dt = PreviousDT();
    std::string file_name = algor_name_ + "_" + size_ + "GB.gz";
    return PathJoin(base_dir_, ip_, prev_dt, bucket_id_, "ckp", file_name);
  }

  std::string CurrentCheckpoint() const {
    std::string file_name = algor_name_ + "_" + size_ + "GB.gz";
    return PathJoin(base_dir_, ip_, dt_, bucket_id_, "ckp", file_name);
  }

 private:

  std::string PreviousDT() const {
    struct tm today_tm = {0};
    today_tm.tm_year = std::stoi(dt_.substr(0, 4));
    today_tm.tm_mon = std::stoi(dt_.substr(4, 2)) - 1; // [0,11]
    today_tm.tm_mday = std::stoi(dt_.substr(6));
    time_t time_sec = mktime(&today_tm) - 24 * 60 * 60;
    struct tm * yest_tm = localtime(&time_sec);
    char buf[260] = {0};
    snprintf(buf, 260, "%d%02d%02d", yest_tm->tm_year, yest_tm->tm_mon + 1,
        yest_tm->tm_mday);
    return buf;
  }

  std::string ip_;
  std::string dt_;
  std::string size_;
  std::string bucket_id_;
  std::string algor_name_;
  std::string base_dir_;
};

#endif
