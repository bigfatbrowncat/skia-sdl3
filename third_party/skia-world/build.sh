cd skia
bin/gn gen out/Static --args='is_official_build=true skia_use_egl=true skia_use_x11=false skia_use_fontconfig=true' # skia_enable_tools=true'
ninja -C out/Static
