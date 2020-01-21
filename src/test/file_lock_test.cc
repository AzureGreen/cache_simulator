//
// Created by oakzhang on 2020/1/20.
//

#include "util/mutex/file_lock.h"

#include <unistd.h>
#include <string>

#include "util/util.h"

int main() {

  int pid = fork();
  if (pid < 0) {
    PrintError("Fail to fork!");
  } else if (pid == 0) {
    // child process
    FileLock fl;
//    sleep(4);
    printf("This is child process!\n");
    int fd = open("/tmp/file_lock_test", O_CREAT | O_RDWR | O_APPEND, 0777);
    std::string buf = "This is child process!\n";
    int n = 7;
    int ret = 0;
    while (n--) {
      ret = write(fd, buf.data(), buf.size());
    }
    printf("Child write %d bytes\n", ret);
    close(fd);
  } else {
    // parent process
    FileLock fl;
    sleep(1);
    printf("This is parent process!\n");
    int fd = open("/tmp/file_lock_test", O_CREAT | O_RDWR | O_APPEND, 0777);
    std::string buf = "This is parent process!\n";
    int n = 7;
    int ret = 0;
    while (n--) {
      ret = write(fd, buf.data(), buf.size());
    }
    printf("Parent write %d bytes\n", ret);
    close(fd);
  }
  return 0;
}

