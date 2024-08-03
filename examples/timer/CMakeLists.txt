cmake_minimum_required(VERSION 3.20)

project(timer)

add_executable(timer
    timer.cpp
)

target_include_directories(timer PUBLIC
    ${basic_SOURCE_DIR}
)

target_link_libraries(timer PUBLIC
    basic
)
