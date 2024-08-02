set -e
export SDL_SRC=`pwd`/SDL
export PREFIX=`pwd`/prefix
export BUILD_SDL=`pwd`/build/sdl

mkdir -p ${PREFIX}
mkdir -p ${BUILD_SDL}
(\
cd ${BUILD_SDL}; \
cmake ${SDL_SRC} -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_PREFIX_PATH=${PREFIX} \
                 -DSDL_STATIC=ON -DSDL_SHARED=OFF \
                 -DSDL_STATIC_PIC=ON -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
                 -DSDL_KMSDRM=ON -DSDL_X11=OFF -DSDL_LIBUDEV=ON \
                 -DSDL_EXAMPLES=ON; \
\
cmake --build .; \
cmake --install . \
)
