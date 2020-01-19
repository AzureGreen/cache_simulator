/**
 * File              : thread_pool_test.cc
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 17.11.2019
 * Last Modified Date: 17.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#include "util/threadpool/thread_pool.h"

#include <iostream>

void sub1() {
  std::cout << "This is sub1, without parameters" << std::endl;
}

void sub2(int p1) {
  std::cout << "This is sub2, with paramter:" << p1 << std::endl;
}

void sub3(int p1, char p2) {
  std::cout << "This is sub3, with paramter:" << p1 << " & " << p2 << std::endl;
}


void test_commit() {
  std::cout << "commit" << std::endl;
  ThreadPool thread_pool(5, 5);
  for (int i = 0; i < 100; i++) {
    thread_pool.Commit(sub1);
    thread_pool.Commit(std::bind(sub2, i));
    thread_pool.Commit(std::bind(sub3, i, i));
    // thread_pool.Commit(sub3, i, i);
  }
}

void test_donothing() {
  std::cout << "do nothing" << std::endl;
  ThreadPool thread_pool(5, 5);
}

int main() {
  test_commit();
  test_donothing();
  return 0;
}

