cmake_minimum_required(VERSION 3.12)
project(request)

set(CMAKE_CXX_STANDARD 11)

add_executable(request request.cpp)

target_link_libraries(request
    PRIVATE
    -lrt
    -lpthread
    simpleLogger
)
