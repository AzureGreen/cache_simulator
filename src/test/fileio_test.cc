/**
 * File              : fileio_test.cc
 * Author            : azuregreen <zhangjiawei1006@gmail.com>
 * Date              : 15.11.2019
 * Last Modified Date: 17.11.2019
 * Last Modified By  : azuregreen <zhangjiawei1006@gmail.com>
 */

// #include <unistd.h>

#include <iostream>
#include <string>


#include "util/fileio/file_reader.h"
#include "util/fileio/file_writer.h"


void TestByLine() {
  std::string read_filepath = "/home/azure/code/cache_simulator/src/test/fileio_test.cc";
  std::string write_filepath = "/home/azure/code/cache_simulator/src/test/fileio_test.cc.gz";

  FileReader reader;
  FileWriter writer;
  reader.OpenFile(read_filepath);
  writer.OpenFile(write_filepath);

  std::cout << "Testing Writer" << std::endl;
  while (!reader.eof()) {
    auto line = reader.ReadLine();
    if (reader.eof()) break;
    writer.WriteLine(line);
  }
  // need flush by force
  writer.Flush();
  std::cout << "Finish Writing" << std::endl;

  std::cout << "Testing Reader" << std::endl;
  reader.OpenFile(read_filepath);

  while (!reader.eof()) {
    auto l = reader.ReadLine();
    std::cout << l << std::endl;
  }
}

void TestByWord() {
  std::string file_name = "./tmp";
  
  FileWriter writer;
  writer.OpenFile(file_name);

  std::cout << "Testing Writer" << std::endl;
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      writer << j << "\t";
    }
    writer << "\n";
  }
  writer.Flush();

  FileReader reader;
  reader.OpenFile(file_name);
  while (!reader.eof()) {
    for (int i = 0; i < 10; i++) {
      size_t n = 0;
      reader >> n;
      if (reader.eof()) break;  // last pos->0xFF
      std::cout << n << "\t";
    }
    std::cout << std::endl;
  }
}

int main() {

  TestByWord();
  
  return 0;
}
