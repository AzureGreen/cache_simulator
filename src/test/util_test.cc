/**
 * File              : util_test.cc
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 16.11.2019
 * Last Modified Date: 16.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */


#include "util/util.h"
#include <string>
#include <iostream>

void PathJoinTest() {
  std::cout << "Testing for PathJoin Function" << std::endl;
  std::cout << PathJoin("src", "test", "util_test.cc") << std::endl;
}

void MakeDirTest() {
  std::cout << "Testing for MakeDir Function" << std::endl;
  std::string path = "/data/test/lalala/test/hahaha";
  MakeDir(path);
}

int main() {
  
  PathJoinTest();
  MakeDirTest();

  return 0;
}


