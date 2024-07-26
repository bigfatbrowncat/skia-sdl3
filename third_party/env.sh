export PREFIX=`pwd`/prefix
export PATH=${PATH}:${PREFIX}/bin
export LD_LIBRARY_PATH=${PREFIX}/lib:${PREFIX}/lib/aarch64-linux-gnu
export PKG_CONFIG_PATH=${PREFIX}/lib/aarch64-linux-gnu/pkgconfig:${PREFIX}/lib/pkgconfig

cd skia-world
source env.sh
cd ..
