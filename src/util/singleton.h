#ifndef UTIL_SINGLETION_H
#define UTIL_SINGLETION_H

template <typename T>
class Singleton {
 public:
  static T &Instance() {
    static T instance;
    return instance;
  }

 protected:
  Singleton<T>() = default;
  Singleton<T>(const Singleton<T>&) = delete;
  Singleton<T>&operator=(const Singleton<T>&) = delete;
};


#endif