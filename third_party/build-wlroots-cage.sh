set -e
export WLROOTS_SRC=`pwd`/wlroots
export CAGE_SRC=`pwd`/cage

export PREFIX=`pwd`/prefix

export BUILD_WLROOTS=`pwd`/build/wlroots
export BUILD_CAGE=`pwd`/build/cage

mkdir -p ${PREFIX}
mkdir -p ${BUILD_WLROOTS}
mkdir -p ${BUILD_CAGE}

( mkdir -p ${BUILD_WLROOTS} &&\
cd ${WLROOTS_SRC} &&\
meson setup ${BUILD_WLROOTS} --prefix=${PREFIX} &&\
cd ${BUILD_WLROOTS} &&\
ninja -C . install \
) && \
\
( mkdir -p ${BUILD_CAGE} &&\
cd ${CAGE_SRC} &&\
meson setup ${BUILD_CAGE} --prefix=${PREFIX} &&\
cd ${BUILD_CAGE} &&\
ninja -C . install \
)
