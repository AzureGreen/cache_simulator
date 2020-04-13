add_library(max_bhr SHARED max_bhr.cc)
add_library(max_bhr_static STATIC max_bhr.cc)

target_link_libraries(max_bhr file_io)
target_link_libraries(max_bhr_static file_io)

