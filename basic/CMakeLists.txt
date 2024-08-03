cmake_minimum_required(VERSION 3.20)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(basic)

add_library(basic SHARED
    SDLGraphAppBase.cpp
    GraphAppImpl.cpp
    basic_api.cpp
)

set(TP_PREFIX "../third_party/prefix")

#set_source_files_properties(basic.cpp PROPERTIES COMPILE_FLAGS "-include ${CMAKE_SOURCE_DIR}/user_app.inc")

target_include_directories(basic PRIVATE
      ${TP_PREFIX}/include
      ${TP_PREFIX}/include/skia
      ${TP_PREFIX}/include/SDL3
)

target_link_directories(basic PRIVATE
      ${TP_PREFIX}/lib
)

target_link_libraries(basic PRIVATE
      skia
      webpmux
      webpdemux
      webp
      jpeg
      png
      freetype
      GL
      EGL
      GLX
      SDL3
      fontconfig
      z
)

# App

#add_executable(basicapp
#    basic.cpp
#)

#target_include_directories(basicapp PUBLIC
#    ${PROJECT_SOURCE_DIR}
#)

#target_link_libraries(basicapp PUBLIC
#    basic
#)
