#include "replace.h"

#include <string>
#include <sstream>
#include <glog/logging.h>

#include "cache/checkpoint_replacement.h"
#include "cache/base/defs.h"
#include "include/defs.h"
#include "include/filemgr.h"
#include "util/util.h"

//struct Performace {
//  size_t hit_bytes;
//  size_t request_bytes;
//};

//#ifdef DEBUG
//#define GOOGLE_STRIP_LOG 1
//#else
//#define GOOGLE_STRIP_LOG 2
//#endif


static std::string FLAGS_task_id = "";

extern "C" {

  void CacheTest(const char * ip,  const char * dt, int bucket_id,
     const char * algor_name, size_t cache_size, Performance *p) {
    algorithm_t algor = ReplacementAlgorithm(algor_name);
    std::string size = std::to_string(cache_size / constant::GIGABYTE);

    FLAGS_task_id = JoinString(ip, "_", dt, "_", bucket_id, "_", algor_name, "_", size, "GB");

//    std::stringstream ss;
//    ss << ip << "_" << dt << "_" << bucket_id << "_" << algor_name << "_" << size << "GB";
//    FLAGS_task_id = ss.str();
//    FLAGS_task_id = std::string(ip) + "_" + std::string(dt) + "_" +
//        std::to_string(bucket_id) + "_" + algor_name + "_" + size + "GB";

    // do some initialization work
    google::InitGoogleLogging(FLAGS_task_id.data());
    FLAGS_log_dir = "/data/cache_simulator/logs/cache";
    LOG(INFO) << "start to simulate " << FLAGS_task_id;

    FileMgr::Instance().Initialize(ip, dt, bucket_id, algor_name, size);

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

    LOG(INFO) << "Finish simulation " << FLAGS_task_id << " hit bytes: " << p->hit_bytes
        << " request bytes: " << p->request_bytes;

    google::ShutdownGoogleLogging();
  }
};