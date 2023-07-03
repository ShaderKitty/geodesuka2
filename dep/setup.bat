REM This file only exists because piggersoft's development tools are retarded

set REPO_ARCH=%1
set DEP_DIR=%2
set BLD_PTH=%3
set PROJECT_TYPE="NMake Makefiles"
set EDIT_FILE=dep\freetype\bld\Windows\include\freetype\config\ftoption.h
REM set PROJECT_TYPE="Visual Studio 17 2022"

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" %REPO_ARCH%
REM call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
call cmake -S %DEP_DIR%/glfw/ -B %DEP_DIR%/glfw/%BLD_PTH%/ -G %PROJECT_TYPE%
call cmake -S %DEP_DIR%/portaudio/ -B %DEP_DIR%/portaudio/%BLD_PTH%/ -G %PROJECT_TYPE% -DPA_BUILD_SHARED=OFF -DPA_DLL_LINK_WITH_STATIC_RUNTIME=OFF
call cmake -S %DEP_DIR%/assimp/ -B %DEP_DIR%/assimp/%BLD_PTH%/ -G %PROJECT_TYPE% -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF
call cmake -S %DEP_DIR%/freetype/ -B %DEP_DIR%/freetype/%BLD_PTH%/ -G %PROJECT_TYPE% -DFT_DISABLE_ZLIB=TRUE -DFT_REQUIRE_ZLIB=FALSE
REM powershell -Command "(gc freetype\bld\win32\include\freetype\config\ftoption.h) -replace '#define FT_CONFIG_OPTION_USE_ZLIB', '//#define FT_CONFIG_OPTION_USE_ZLIB' | Out-File -encoding ASCII freetype\bld\win32\include\freetype\config\ftoption.h"
