/**
 * File              : util.h
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 15.11.2019
 * Last Modified Date: 15.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */


#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

// string
inline std::vector<std::string> Split(const std::string &str, const char delim) {
  std::vector<std::string> res;
  size_t cur, last = 0;
  auto push = [&]() {
    if (cur - last > 0) {
      res.push_back(str.substr(last, cur - last));
    }
  };
  for (cur = 0; cur < str.size(); cur++) {
    if (str[cur] == delim) {
      push();
      last = cur + 1;
    }
  }
  // handle last
  push();
  return res;
}

inline bool EndWith(const std::string &s, const std::string &sub) {
  return s.find(sub) == (s.length() - sub.length());
}

inline bool StartWith(const std::string &s, const std::string &sub) {
  return s.find(sub) == 0;
}

template <typename T>
int JoinPath_aux(std::stringstream &ss, T item) {
  ss << item;
  return 0;
}

template <typename ...Args>
inline std::string JoinString(Args... args) {
  static std::stringstream ss;
  ss.clear();
  std::initializer_list<int>{JoinPath_aux(ss, std::forward<Args>(args))...};  // unpacking
  return ss.str();
}

inline void PrintError [[ noreturn ]] (const std::string &msg) {
  fprintf(stderr, "%s\n", msg.data());
  exit(EXIT_FAILURE);
}

// file
inline std::vector<std::string> GetChildFiles(const std::string &parent_dir) {
  std::vector<std::string> files;
  DIR *dp = nullptr;
  struct dirent *dirp = nullptr;
  dp = opendir(parent_dir.data());
  if (dp == nullptr) {
    PrintError("Open dir " + parent_dir + " failed");
  }
  while ((dirp = readdir(dp)) != nullptr) {
    if (StartWith(dirp->d_name, "access")) {
      files.push_back(parent_dir + "/" + dirp->d_name);
    }
  }
  std::sort(files.begin(), files.end());
  return files;
}

inline std::string FileDirectory(const std::string &filename) {
  size_t len = filename.size() - 1;
  while (filename[len] != '/') {
    len--;
  }
  return filename.substr(0, len);
}

inline std::string FileName(const std::string &filename) {
  size_t len = filename.size() - 1;
  while (filename[len] != '/') {
    len--;
  }
  return filename.substr(len + 1);
}

inline bool PathExist(const std::string &filename) {
  return access(filename.data(), F_OK) == 0;
}

inline bool MakeDir(const std::string &dir_path) {
  if (PathExist(dir_path)) return true;
  auto &&split_paths = Split(dir_path, '/');
  std::string path = "";
  for (auto &i : split_paths) {
    path += "/" + i;
    if (!PathExist(path)) {
      if (mkdir(path.data(), 0755) == -1) {
        PrintError("Fail to mkdir: " + path);
      }
    }
  }
  return true;
}

template<typename ...Args>
inline std::string PathJoin(Args... args) {
  std::string path_name;
  auto f = [&](const std::string & part) { 
    if (!path_name.empty()) {
      path_name += "/";
    }
    path_name += part;
    return 0;
  };
  std::initializer_list<int>{f(std::forward<Args>(args))...};  // unpacking
  return path_name;
}

inline void RewindIs(std::istream &is) {
  is.clear();
  is.seekg(0, std::ios::beg);
}

inline size_t FileLength(std::istream &is) {
  size_t res = 0;
  std::string line;
  while (getline(is, line)) {
    res++;
  }
  RewindIs(is);
  return res;
}

#endif  // UTIL_H

