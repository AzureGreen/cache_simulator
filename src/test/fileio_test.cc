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


int main() {
  
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

  return 0;
}
