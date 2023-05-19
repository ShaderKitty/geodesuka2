# Creates build files for linux
cmake -S glfw/ -B glfw/bld/linux/ -G "Unix Makefiles"
cmake -S portaudio/ -B portaudio/bld/linux/ -G "Unix Makefiles" -DPA_BUILD_SHARED=OFF
cmake -S assimp/ -B assimp/bld/linux/ -G "Unix Makefiles" -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ZLIB=ON
cmake -S freetype/ -B freetype/bld/linux/ -G "Unix Makefiles" -DFT_DISABLE_ZLIB=TRUE -DFT_REQUIRE_ZLIB=FALSE
# sed -i 's/#define FT_CONFIG_OPTION_USE_ZLIB/\/\/#define FT_CONFIG_OPTION_USE_ZLIB/' freetype/bld/linux/include/freetype/config/ftoption.h
sed -i 's/#define FT_CONFIG_OPTION_USE_BROTLI/\/\/#define FT_CONFIG_OPTION_USE_BROTLI/' freetype/bld/linux/include/freetype/config/ftoption.h

# #define FT_CONFIG_OPTION_USE_ZLIB
# #define FT_CONFIG_OPTION_USE_BROTLI