REM This file only exists because piggersoft's development tools are retarded

set REPO_ARCH=%1
set DEP_DIR=dep
set BLD_PTH=bld/Windows/%REPO_ARCH%
REM set PROJECT_TYPE="NMake Makefiles"
set PROJECT_TYPE="Visual Studio 17 2022"
set EDIT_FILE=dep/freetype/%BLD_PTH%/include/freetype/config/ftoption.h

mkdir dep/

git clone https://github.com/glfw/glfw.git %DEP_DIR%/glfw
git clone https://github.com/PortAudio/portaudio.git %DEP_DIR%/portaudio
git clone https://github.com/assimp/assimp.git dep/assimp
git clone https://gitlab.freedesktop.org/freetype/freetype.git %DEP_DIR%/freetype
git clone https://github.com/ShaderKitty/freeimage.git %DEP_DIR%/freeimage
git clone https://github.com/g-truc/glm.git %DEP_DIR%/glm
(cd %DEP_DIR%/glfw && git checkout 3.3.8)
cd ../..
(cd %DEP_DIR%/portaudio && git checkout v19.7.0)
cd ../..
(cd %DEP_DIR%/assimp && git checkout v5.2.5)
cd ../..
(cd %DEP_DIR%/freetype && git checkout VER-2-12-1)
cd ../..
(cd %DEP_DIR%/glm && git checkout 0.9.9.8)
cd ../..

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" %REPO_ARCH%
REM call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
call cmake -S %DEP_DIR%/glfw/ -B %DEP_DIR%/glfw/%BLD_PTH%/ -G %PROJECT_TYPE%
call cmake -S %DEP_DIR%/portaudio/ -B %DEP_DIR%/portaudio/%BLD_PTH%/ -G %PROJECT_TYPE% -DPA_BUILD_SHARED=OFF -DPA_DLL_LINK_WITH_STATIC_RUNTIME=OFF
call cmake -S %DEP_DIR%/assimp/ -B %DEP_DIR%/assimp/%BLD_PTH%/ -G %PROJECT_TYPE% -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF
call cmake -S %DEP_DIR%/freetype/ -B %DEP_DIR%/freetype/%BLD_PTH%/ -G %PROJECT_TYPE% -DFT_DISABLE_ZLIB=TRUE -DFT_REQUIRE_ZLIB=FALSE
powershell -Command "(gc %EDIT_FILE%) -replace '#define FT_CONFIG_OPTION_USE_ZLIB', '//#define FT_CONFIG_OPTION_USE_ZLIB' | Out-File -encoding ASCII %EDIT_FILE%"
