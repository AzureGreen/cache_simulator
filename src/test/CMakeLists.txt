# test for lib
add_executable(fileio_test fileio_test.cc)
add_executable(util_test util_test.cc)
add_executable(file_lock_test file_lock_test.cc)
add_executable(thread_pool_test thread_pool_test.cc )

target_link_libraries(fileio_test file_io)
target_link_libraries(thread_pool_test thread_pool)


# another test
add_executable(replace_test replace_test.cc)
target_link_libraries(replace_test replace)

