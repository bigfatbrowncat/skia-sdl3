cmake_minimum_required(VERSION 3.20)
project(basic)

add_executable(basic SDLGraphAppBase.cpp)
set(TP_PREFIX "../third_party/prefix")

target_include_directories(basic PUBLIC
      ${TP_PREFIX}/include
      ${TP_PREFIX}/include/skia
      ${TP_PREFIX}/include/SDL3
)

target_link_directories(basic PUBLIC
      ${TP_PREFIX}/lib
)

target_link_libraries(basic PUBLIC
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
