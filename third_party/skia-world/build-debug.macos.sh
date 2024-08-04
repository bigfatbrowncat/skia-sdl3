cd skia

# Need to run /Applications/Python\ 3.10/Install\ Certificates.command before starting

python3 bin/fetch-gn
python3 tools/git-sync-deps
python3 bin/fetch-ninja

bin/gn gen ../out/Static --args='is_official_build=false skia_use_gl=true skia_use_system_expat=false skia_use_system_icu=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_fontconfig=true'
ninja -C ../out/Static -t compdb > ../compile_commands.json
ninja -C ../out/Static 
#-j 1

#--args="is_official_build=true skia_use_system_expat=false skia_use_system_icu=false skia_use_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_libwebp=false extra_cflags_cc=[\"-frtti\"]"