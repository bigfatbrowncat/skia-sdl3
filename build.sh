set -e
cd third_party
source env.sh
./build.sh

cd ../skia-sdl3
./build.sh
