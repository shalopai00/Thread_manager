cmake_minimum_required(VERSION 3.10)
project(OpenMP_for_laba5)

set(CMAKE_CXX_STANDARD 17)

add_executable(OpenMP_for_laba5
        main.cpp
        thread_manager.cpp
        Queue.cpp
        task.cpp
        request_queue.cpp
        request_queue.h
)

target_link_libraries(OpenMP_for_laba5 pthread)