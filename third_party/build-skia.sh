set -e
cd skia-world
source env.sh
./build.sh
cd ..

mkdir -p prefix/lib

cp skia-world/out/Static/*.a prefix/lib/

mkdir -p prefix/include/skia/
cd skia-world/skia

cp_dir() {
    mkdir -p $(dirname "$2") && cp "$1" "$2"
}
export -f cp_dir

find include -name *.h \
         -exec echo "Copying {} to prefix/include/skia/{}" \; \
         -exec bash -c 'cp_dir "{}" "../../prefix/include/skia/{}"' \;

find modules -name *.h \
         -exec echo "Copying {} to prefix/include/skia/{}" \; \
         -exec bash -c 'cp_dir "{}" "../../prefix/include/skia/{}"' \;

find src -name *.h \
         -exec echo "Copying {} to prefix/include/skia/{}" \; \
         -exec bash -c 'cp_dir "{}" "../../prefix/include/skia/{}"' \;
