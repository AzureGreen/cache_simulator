/**
 * File              : max_bhr.cc
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 18.02.2020
 * Last Modified Date: 17.03.2020
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */
#include "statistic/max_bhr.h"

#include <iomanip>
#include <string>
#include <unordered_map>

#include "include/filemgr.h"
#include "util/fileio/file_reader.h"
#include "util/fileio/file_writer.h"
#include "parse/feature_parser.h"
#include "util/util.h"



template <typename T>
void LoadPreStatMap(const std::string &ckp_path, 
    std::unordered_map<T, size_t> &stat_map, 
    double &tot_req_bytes, double &tot_uniq_bytes) {

  if (ckp_path.empty()) {
    return ;
  }
  FileReader reader;
  if (!reader.OpenFile(ckp_path)) {
    return ;
  }
  reader >> tot_req_bytes >> tot_uniq_bytes;
  while (!reader.eof()) {
    T key;
    size_t size = 0;
    reader >> key;
    reader >> size;
    if (reader.eof()) break;
    stat_map[key] = size;
  }
}


template<typename T>
void DumpCurStatMap(const std::string &ckp_path,
    const std::unordered_map<T, size_t> &stat_map,
    double &tot_req_bytes, double &tot_uniq_bytes) {
  FileWriter writer;
  if (!writer.OpenFile(ckp_path)) {
    return;
  }
  writer << tot_req_bytes << "\t" << tot_uniq_bytes << "\n";
  for (auto &p : stat_map) {
    writer << p.first << "\t" << p.second << "\n";
  }
}


extern "C" {

void Statistic(size_t task_id, const char *ip, const char *dt, 
    int bucket, Metrics *metrics) {
  // do some initialization work
  FileMgr::Instance().Initialize(task_id, ip, dt, bucket);

  // trace files
  std::string trace_dir = FileMgr::Instance().TraceDir();
  auto trace_files = GetChildFiles(trace_dir);
  // previous checkpoint file
  std::string prev_ckf = FileMgr::Instance().PrevStatCkp();
  if (!PathExist(prev_ckf)) prev_ckf = "";

  std::string cur_ckf = FileMgr::Instance().CurStatCkp();

  std::unordered_map<size_t, size_t> tot_stat_map; 
  double last_tot_req_bytes = 0;
  double last_tot_uniq_bytes = 0;
  LoadPreStatMap<size_t>(prev_ckf, tot_stat_map, last_tot_req_bytes, 
      last_tot_uniq_bytes);

  double request_bytes = 0;
  std::unordered_map<size_t, size_t> stat_map;

  for (auto &file : trace_files) {
    FeatureParser parser;
    if (!parser.OpenFile(file)) {
      PrintError("Fail to open file: " + file);
    }
    int l = 0;
    try {
      while (parser.ParseLine()) {
        request_bytes += parser.size();
        if (!tot_stat_map.count(parser.key())) {
          tot_stat_map[parser.key()] = parser.size();
        }
        if (!stat_map.count(parser.key())) {
          stat_map[parser.key()] = parser.size();
        }
      }
      l++;
    } catch(std::out_of_range &e) {
      printf("%s\t line:%d\n", file.data(), l);
      PrintError("Fail to parse file: " + file);
    }
  }

  double file_bytes = 0;
  for (auto &p : stat_map) {
    file_bytes += p.second;
  }
  double total_uniq_bytes = 0;
  for (auto &p : tot_stat_map) {
    total_uniq_bytes += p.second;
  }
  double total_request_bytes = last_tot_req_bytes + request_bytes;

  metrics->uniq_bytes = file_bytes;
  metrics->req_bytes = request_bytes;
  metrics->tot_uniq_bytes = total_uniq_bytes; 
  metrics->tot_req_bytes = total_request_bytes;
 
  // metrics->uniq_bytes = file_bytes / constant::GIGABYTE;
  // metrics->req_bytes = request_bytes / constant::GIGABYTE;
  // metrics->tot_uniq_bytes = total_uniq_bytes / constant::GIGABYTE; 
  /* metrics->tot_req_bytes = total_request_bytes / constant::GIGABYTE; */
  // metrics->avg_hy_ratio = total_uniq_bytes / total_request_bytes; 
  // metrics->daily_hy_ratio = (total_uniq_bytes - last_tot_uniq_bytes) / request_bytes;

  // dump current stat_map
  DumpCurStatMap(cur_ckf, tot_stat_map, total_request_bytes, total_uniq_bytes);
}
};
