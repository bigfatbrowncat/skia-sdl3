cmake_minimum_required(VERSION 3.20)

project(startup)

add_executable(startup
    startup.cpp
)

set_source_files_properties(startup.cpp PROPERTIES COMPILE_FLAGS "-include ${basic_SOURCE_DIR}/basic.inc")

target_include_directories(startup PUBLIC
    ${basic_SOURCE_DIR}
)

target_link_libraries(startup PUBLIC
    basic
)
