set -e
cd skia-world
source env.sh
./build-debug.linux.sh
cd ..

mkdir -p prefix/lib

#cp skia-world/out/Static/*.a prefix/lib/

mkdir -p prefix/include/skia/

cp_dir() {
    abs1=$(readlink -m "$1")
    mkdir -p $(dirname "$abs") && ln -fs "$abs1" "$2"
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
