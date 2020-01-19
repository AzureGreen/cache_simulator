/**
 * File              : thread_pool.cc
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 17.11.2019
 * Last Modified Date: 29.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#include "util/threadpool/thread_pool.h"

// 这里设计的比较糟糕，BlockingQueue和ThreadPool紧耦合了

template <typename Task>
void BlockingQueue<Task>::Push(Task &task) {
  std::unique_lock<std::mutex> lck(mutex_);
  push_cv_.wait(lck, [this] { return !Full(); });
  task_queue_.push(std::move(task));
  // wake a thread (modify shared variable(condition_variable) and notify)
  pop_cv_.notify_one();
}

template <typename Task>
Task BlockingQueue<Task>::Pop() {
  std::unique_lock<std::mutex> lck(mutex_);
  pop_cv_.wait(lck, [this] { return !Empty() || break_signal_; });
  Task task;
  if (!Empty()) {
    task = task_queue_.front();
    task_queue_.pop();
    push_cv_.notify_one();
  }
  return task;
}

template <typename Task>
void BlockingQueue<Task>::Shutdown() {
  break_signal_ = true;
  pop_cv_.notify_all();
}


ThreadPool::ThreadPool(size_t thread_num, size_t queue_num)
  : //stop_(false),
    max_thread_num_(thread_num),
    blocking_queue_(queue_num),
    stop_(false) {

  for (size_t i = 0; i < max_thread_num_; i++) {
    threads_.emplace_back([this] {
      while (true) {
        Task task = blocking_queue_.Pop();
        if (task) task();
        if (stop_ && blocking_queue_.Empty()) break;
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  stop_ = true;
  blocking_queue_.Shutdown();
  for (auto &thread : threads_) {
    thread.join();
  }
}

// outter bind
void ThreadPool::Commit(Task task) {
  // may be block main thread
  blocking_queue_.Push(task);
}

// inner bind
/* template <typename T, typename ...Args> */
// void ThreadPool::Commit(T func, Args&& ...args) {
  // auto task = std::bind(func, std::forward<Args>(args)...); 
  // blocking_queue_.Push(task);
// }

