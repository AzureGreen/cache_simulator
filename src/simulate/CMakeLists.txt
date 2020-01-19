add_library(replace SHARED replace.cc)

find_package(Threads)
target_link_libraries(replace file_io libglog.a libgflags.a ${CMAKE_THREAD_LIBS_INIT})
