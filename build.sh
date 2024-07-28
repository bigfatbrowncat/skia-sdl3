set -e
cd third_party
source env.sh
./build.sh

cd ../basic
./build.sh
