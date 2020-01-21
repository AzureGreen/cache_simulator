# CDN Cache Simulator

### Introduction

This project is the **cache simulation** part of the CDN cache offline analysis. 

This CDN cache has a chunk storage feature that all files larger than 1 MB are split into multiple chunks at a granularity of 1 MB and parts smaller than 1 MB are divided at 32 KB. Each chunk is treated as a separate file. Therefore, this requirement was also followed when implementing this simulator.

The core part of this simulator contains the implementation of various common cache replacement algorithms, such as : `FIFO`, `LRU`, `2Q`, `MQ`, `SLRU`, `ARC`, `LIRS`, `Belady`, etc.

### Dependencies

Compiler: GCC 4.8.5+

Build System: CMake 3.9+

Third party libraries: Boost-static, (gflags, glog if not build `libreplace.so`, it is not necessary)

### Build

Generate makefile:

```shell
cd src && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
```

Build:

```shell
make
```

### Usage

Because this is a custom simulation system, if you need to use it, you can change the part of the cache replacement algorithm get method and insert method to handle the file size according to your needs. This project only involves chunks with a size of 1 MB and chunks of 32 KB or less.