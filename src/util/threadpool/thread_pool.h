/**
 * File              : thread_pool.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 16.11.2019
 * Last Modified Date: 16.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

static const size_t FLAGS_max_blocking_queue_size = 5;

template <typename Task>
class BlockingQueue {
 public:
  BlockingQueue(size_t size = FLAGS_max_blocking_queue_size)
    : max_size_(size), break_signal_(false) {}
  ~BlockingQueue() {}

  inline bool Full() const { return max_size_ > 0 && task_queue_.size() >= max_size_; }

  inline bool Empty() const { return task_queue_.empty(); }

  inline size_t Size() const { return task_queue_.size(); }

  void Push(Task &task);

  Task Pop();

  void Shutdown();

 private:
  size_t max_size_;
  bool break_signal_;
  std::queue<Task> task_queue_;
  std::mutex mutex_;
  std::condition_variable push_cv_;
  std::condition_variable pop_cv_;
};


class ThreadPool {
 public:
   using Task = std::function<void()>;

  ThreadPool(size_t thread_num, size_t queue_num);

  ~ThreadPool();

  void Commit(Task task);

/*   template <typename T, typename ...Args> */
  // void Commit(T func, Args&& ...args);

 private:
  size_t max_thread_num_;
  std::vector<std::thread> threads_;
  BlockingQueue<Task> blocking_queue_;
  bool stop_;
};

#endif  // THREAD_POOL_H
