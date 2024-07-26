export TP_PREFIX=../third_party/prefix
g++ skia-sdl3.cpp \
      -o skia-sdl3 \
      -I${TP_PREFIX}/include \
      -I${TP_PREFIX}/include/skia \
      -I${TP_PREFIX}/include/SDL3 \
      -L${TP_PREFIX}/lib \
      \
      -lskia \
      -lwebpmux \
      -lwebpdemux \
      -lwebp \
      -ljpeg \
      -lpng \
      -lfreetype \
      -lGL \
      -lEGL \
      -lGLX \
      -lSDL3 \
      -lfontconfig \
      -lz
