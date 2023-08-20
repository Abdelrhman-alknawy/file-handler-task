cmake_minimum_required(VERSION 3.12)
project(file_handler)

set(CMAKE_CXX_STANDARD 11)

add_executable(fh fh.cpp)

target_link_libraries(fh
    PRIVATE
    -lrt
    -lpthread
    simpleLogger
)
