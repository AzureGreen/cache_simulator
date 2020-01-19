# this is threadpool.a
add_library(thread_pool STATIC threadpool/thread_pool.cc)

find_package(Threads)
target_link_libraries(thread_pool ${CMAKE_THREAD_LIBS_INIT})

# this is file_io.a
add_library(file_io STATIC fileio/file_reader.cc fileio/file_writer.cc)

find_package(Boost REQUIRED COMPONENTS system iostreams)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(file_io ${Boost_LIBRARIES})

#add_library(util util.cc)
