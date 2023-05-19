cmake -S glfw/ -B glfw/bld/win32/ -G "Visual Studio 17 2022"
cmake -S portaudio/ -B portaudio/bld/win32/ -G "Visual Studio 17 2022" -DPA_BUILD_SHARED=OFF -DPA_DLL_LINK_WITH_STATIC_RUNTIME=OFF
cmake -S assimp/ -B assimp/bld/win32/ -G "Visual Studio 17 2022" -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF
cmake -S freetype/ -B freetype/bld/win32/ -G "Visual Studio 17 2022" -DFT_DISABLE_ZLIB=TRUE -DFT_REQUIRE_ZLIB=FALSE
powershell -Command "(gc freetype\bld\win32\include\freetype\config\ftoption.h) -replace '#define FT_CONFIG_OPTION_USE_ZLIB', '//#define FT_CONFIG_OPTION_USE_ZLIB' | Out-File -encoding ASCII freetype\bld\win32\include\freetype\config\ftoption.h"