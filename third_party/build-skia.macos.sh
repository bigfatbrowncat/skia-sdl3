set -e
cd skia-world
source env.sh
./build-debug.macos.sh
cd ..

mkdir -p prefix/lib

#cp skia-world/out/Static/*.a prefix/lib/

mkdir -p prefix/include/skia/

cp_dir() {
    abs1=$(readlink -f "$1")
    mkdir -p $(dirname "$2") && ln -fs "$abs1" "$2"
}
export -f cp_dir

cd skia-world/out/Static
find . -name "*.a" \
         -exec echo "Copying {} to prefix/lib/{}" \; \
         -exec bash -c 'cp_dir "{}" "../../../prefix/lib/{}"' \;

cd ../../skia
find include -name *.h \
         -exec echo "Copying {} to prefix/include/skia/{}" \; \
         -exec bash -c 'cp_dir "{}" "../../prefix/include/skia/{}"' \;

find modules -name *.h \
         -exec echo "Copying {} to prefix/include/skia/{}" \; \
         -exec bash -c 'cp_dir "{}" "../../prefix/include/skia/{}"' \;

find src -name *.h \
         -exec echo "Copying {} to prefix/include/skia/{}" \; \
         -exec bash -c 'cp_dir "{}" "../../prefix/include/skia/{}"' \;
