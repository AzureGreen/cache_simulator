//
// Created by oakzhang on 2020/1/20.
//

#ifndef CDN_CACHE_ANALYSIS_FILE_LOCK_H
#define CDN_CACHE_ANALYSIS_FILE_LOCK_H

#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

class FileLock {
 public:
  FileLock() {
    fd_ = open("/tmp/file_lock", O_RDONLY | O_CREAT, 0777);
    flock(fd_, LOCK_EX);
  }

  ~FileLock() {
    flock(fd_, LOCK_UN);
    close(fd_);
  }
 private:
  int fd_;
};


#endif //CDN_CACHE_ANALYSIS_FILE_LOCK_H
