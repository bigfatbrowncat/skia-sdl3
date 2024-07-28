set -e
mkdir -p ../build
export TP_PREFIX=../third_party/prefix
g++ basic.cpp -O0 -g3 -ggdb3 \
      -o ../build/basic \
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
