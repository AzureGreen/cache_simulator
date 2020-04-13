#include "replace.h"

#include <string>
#include <sstream>
#include <glog/logging.h>

#include "cache/checkpoint_replacement.h"
#include "cache/base/defs.h"
#include "include/defs.h"
#include "include/filemgr.h"
#include "util/util.h"
#include "util/mutex/file_lock.h"

//struct Performace {
//  size_t hit_bytes;
//  size_t request_bytes;
//};

//#ifdef DEBUG
//#define GOOGLE_STRIP_LOG 1
//#else
//#define GOOGLE_STRIP_LOG 2
//#endif


static std::string FLAGS_task_id;

extern "C" {

void CacheTest(size_t task_id, const char * ip,  const char * dt, int bucket_id,
   const char * algor_name, size_t cache_size, Performance *p) {
  algorithm_t algor = ReplacementAlgorithm(algor_name);
  std::string size = std::to_string(cache_size / constant::GIGABYTE);

  // FLAGS_task_id = std::to_string(task_id);
  // FLAGS_task_id = JoinString(task_id, ip, "_", dt, "_", bucket_id, "_", algor_name, "_", size, "GB");

  // do some initialization work
  FileMgr::Instance().Initialize(task_id, ip, dt, bucket_id, algor_name, size);

  // google::InitGoogleLogging(FLAGS_task_id.data());
  // FLAGS_log_dir = FileMgr::Instance().LogDir();
  // if (!PathExist(FLAGS_log_dir)) {
    // // here be careful about process lock
    // FileLock fl;
    // MakeDir(FLAGS_log_dir);
  // }
  // LOG(INFO) << "start to simulate " << FLAGS_task_id;

  // trace files
  std::string trace_dir = FileMgr::Instance().TraceDir();
  auto trace_files = GetChildFiles(trace_dir);
  // previous checkpoint file
  std::string prev_ckf = FileMgr::Instance().PrevCheckpoint();
  if (!PathExist(prev_ckf)) prev_ckf = "";

  std::string cur_ckf = FileMgr::Instance().CurrentCheckpoint();

  CacheTester tester(algor, cache_size);

  tester.Simulate(trace_files, prev_ckf, cur_ckf, p->hit_bytes,
      p->request_bytes);

  // LOG(INFO) << "Finish simulation " << FLAGS_task_id;
  // LOG(INFO) << " hit bytes: " << p->hit_bytes << " request bytes: " << p->request_bytes;

  // google::ShutdownGoogleLogging();
}
};
