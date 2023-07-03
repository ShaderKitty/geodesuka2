cmake -S $(DEP_DIR)/glfw/ -B $(DEP_DIR)/glfw/bld/$(DISC)/ -G $(PROJECT_TYPE)
cmake -S $(DEP_DIR)/portaudio/ -B $(DEP_DIR)/portaudio/bld/$(DISC)/ -G $(PROJECT_TYPE) -DPA_BUILD_SHARED=OFF
cmake -S $(DEP_DIR)/assimp/ -B $(DEP_DIR)/assimp/bld/$(DISC)/ -G $(PROJECT_TYPE) -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ZLIB=ON
cmake -S $(DEP_DIR)/freetype/ -B $(DEP_DIR)/freetype/bld/$(DISC)/ -G $(PROJECT_TYPE) -DFT_DISABLE_ZLIB=TRUE -DFT_REQUIRE_ZLIB=FALSE
sed -i 's/#define FT_CONFIG_OPTION_USE_BROTLI/\/\/#define FT_CONFIG_OPTION_USE_BROTLI/' freetype/bld/$(DISC)/include/freetype/config/ftoption.h