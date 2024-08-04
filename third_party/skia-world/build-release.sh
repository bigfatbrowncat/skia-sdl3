cd skia

python3 bin/fetch-gn
python3 tools/git-sync-deps
python3 bin/fetch-ninja

bin/gn gen ../out/Static --args='is_official_build=true skia_use_egl=true skia_use_x11=false skia_use_fontconfig=true'
ninja -C ../out/Static -t compdb > ../compile_commands.json
ninja -C ../out/Static
