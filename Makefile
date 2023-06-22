# GLFW = -lglfw -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl

# COMPILER = gcc, cl 
# MODE = Release, Debug
# ARCH = x32, x64

ifeq ($(COMPILER),gcc)
	LNK = ld
	LIB = ar -rcs
	CFLGS = -std=c++17 -pthread
	ifeq ($(MODE),Debug)
		CFLGS += -g
	endif
else ifeq ($(COMPILER),cl)
	LNK = link
	LIB = lib
	CFLGS = /NOLOGO
	ifeq ($(MODE),Debug)
		CFLGS += /DEBUG
	endif
else 
# Error, Unknown Compiler

endif

ifeq ($(OS),Windows_NT)
	PLATFORM_OS = Windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		PLATFORM_OS = Linux
	endif
    ifeq ($(UNAME_S),Darwin)
		PLATFORM_OS = MacOS
    endif
endif

ifeq ($(PLATFORM_OS),Windows)

	VULKAN_INC = /I "$(VK_SDK_PATH)\Include\"

	VULKAN_LIB = vulkan-1.lib

	ifeq ($(MODE), Release)
		GLSLANG_LIB = \
		GenericCodeGen.lib \
		glslang-default-resource-limits.lib \
		glslang.lib \
		HLSL.lib \
		MachineIndependent.lib \
		OGLCompiler.lib \
		OSDependent.lib \
		shaderc.lib \
		shaderc_combined.lib \
		shaderc_shared.lib \
		shaderc_util.lib

		SPIRV_LIB = \
		spirv-cross-c-shared.lib \
		spirv-cross-c.lib \
		spirv-cross-core.lib \
		spirv-cross-cpp.lib \
		spirv-cross-glsl.lib \
		spirv-cross-hlsl.lib \
		spirv-cross-msl.lib \
		spirv-cross-reflect.lib \
		spirv-cross-util.lib \
		SPIRV-Tools-diff.lib \
		SPIRV-Tools-link.lib \
		SPIRV-Tools-lint.lib \
		SPIRV-Tools-opt.lib \
		SPIRV-Tools-reduce.lib \
		SPIRV-Tools-shared.lib \
		SPIRV-Tools.lib \
		SPIRV.lib \
		SPVRemapper.lib
	else
		GLSLANG_LIB = \
		GenericCodeGend.lib \
		glslang-default-resource-limitsd.lib \
		glslangd.lib \
		HLSLd.lib \
		MachineIndependentd.lib \
		OGLCompilerd.lib \
		OSDependentd.lib \
		shadercd.lib \
		shaderc_combinedd.lib \
		shaderc_sharedd.lib \
		shaderc_utild.lib

		SPIRV_LIB = \
		spirv-cross-c-sharedd.lib \
		spirv-cross-cd.lib \
		spirv-cross-cored.lib \
		spirv-cross-cppd.lib \
		spirv-cross-glsld.lib \
		spirv-cross-hlsld.lib \
		spirv-cross-msld.lib \
		spirv-cross-reflectd.lib \
		spirv-cross-utild.lib \
		SPIRV-Tools-diffd.lib \
		SPIRV-Tools-linkd.lib \
		SPIRV-Tools-lintd.lib \
		SPIRV-Tools-optd.lib \
		SPIRV-Tools-reduced.lib \
		SPIRV-Tools-sharedd.lib \
		SPIRV-Toolsd.lib \
		SPIRVd.lib \
		SPVRemapperd.lib
	endif

	GLFW_INC = /I "dep/glfw/include/"
	PORTAUDIO_INC = /I "dep/portaudio/include/"
	ASSIMP_INC = /I "dep/assimp/bld/win32/include/" /I "dep/assimp/include/"
	FREETYPE_INC = /I "dep/freetype/bld/win32/include/" /I "dep/freetype/include/"
	FREEIMAGE_INC = /I "dep/freeimage/Source/"

	GLFW_LIB = /LIBPATH:"dep/glfw/bld/win32/src/"
	PORTAUDIO_LIB = /LIBPATH:"dep/portaudio/bld/win32/"
	ASSIMP_LIB = /LIBPATH:"dep/assimp/bld/win32/lib/"
	FREETYPE_LIB = /LIBPATH:"dep/freetype/bld/win32/"
	FREEIMAGE_LIB = /LIBPATH:"dep/freeimage/"

	GLFW_LIB = /LIBPATH:"dep/glfw/bld/win32/src/" glfw3.lib
	PORTAUDIO_LIB = /LIBPATH:"dep/portaudio/bld/win32/" portaudio_static_x64.lib
	FREEIMAGE_LIB = /LIBPATH:"dep/freeimage/" FreeImageLib.lib
	ifeq ($(MODE),Release)

	else
		ASSIMP_LIB = /LIBPATH:"dep/assimp/bld/win32/lib/" assimp-vc143-mtd.lib
		FREETYPE_LIB = /LIBPATH:"dep/freetype/bld/win32/"
	endif

	PROJECT_TYPE = "NMake Makefiles"

	PORTAUDIO_CMAKE_OPTION = -DPA_BUILD_SHARED=OFF -DPA_DLL_LINK_WITH_STATIC_RUNTIME=OFF
	ASSIMP_CMAKE_OPTION = -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF
	FREETYPE_CMAKE_OPTION = -DFT_DISABLE_ZLIB=TRUE -DFT_REQUIRE_ZLIB=FALSE

	ENV_SETUP := C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat amd64

else ifeq ($(PLATFORM_OS), Linux)
	X11_LIB = \
	-lXrandr \
	-lXxf86vm \
	-lXi \
	-lXinerama \
	-lX11 \
	-lrt \
	-ldl

	ALSA_LIB = \
	-lrt \
	-lm \
	-lasound
	
	OPENGL_LIB = -lGL

	VULKAN_LIB = -lvulkan

	GLSLANG_LIB = \
	-lGenericCodeGen \
	-lglslang \
	-lglslang-default-resource-limits \
	-lHLSL \
	-lMachineIndependent \
	-lOGLCompiler \
	-lOSDependent \
	-lshaderc \
	-lshaderc_combined

	SPIRV_LIB = \
	-lSPIRV \
	-lspirv-cross-c \
	-lspirv-cross-core \
	-lspirv-cross-cpp \
	-lspirv-cross-glsl \
	-lspirv-cross-hlsl \
	-lspirv-cross-msl \
	-lspirv-cross-reflect \
	-lspirv-cross-util \
	-lSPIRV-Tools \
	-lSPIRV-Tools-diff \
	-lSPIRV-Tools-link \
	-lSPIRV-Tools-lint \
	-lSPIRV-Tools-opt \
	-lSPIRV-Tools-reduce \
	-lSPVRemapper

	PROJECT_TYPE = "Unix Makefiles"

	PORTAUDIO_CMAKE_OPTION = -DPA_BUILD_SHARED=OFF
	ASSIMP_CMAKE_OPTION = -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ZLIB=ON
	FREETYPE_CMAKE_OPTION = -DFT_DISABLE_ZLIB=TRUE -DFT_REQUIRE_ZLIB=FALSE

	ENV_SETUP :=

else ifeq ($(PLATFORM_OS),MacOS)

	ENV_SETUP :=

else 
# Error, Unknown OS

endif

SYS_INC = $(VULKAN_INC)
SYS_LIB = $(GLSLANG_LIB) $(SPIRV_LIB) $(VULKAN_LIB) $(OPENGL_LIB)

DEP_INC = $(FREEIMAGE_INC) $(FREETYPE_INC) $(ASSIMP_INC) $(PORTAUDIO_INC) $(GLFW_INC)
DEP_LIB = $(FREEIMAGE_LIB) $(FREETYPE_LIB) $(ASSIMP_LIB) $(PORTAUDIO_LIB) $(GLFW_LIB)

INC = -Iinc $(DEP_INC) $(SYS_INC)
LIB = $(DEP_LIB) $(SYS_LIB)

# Source Paths
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.cpp)

# Path substitution for all compiled objects.
OBJ_DIR := obj/$(PLATFORM_OS)/$(MODE)
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

test:
	echo $(VK_SDK_PATH)

dirs: 
	mkdir bin/
	mkdir obj/
	mkdir lib/
	mkdir bin/$(PLATFORM_OS)/
	mkdir obj/$(PLATFORM_OS)/
	mkdir lib/$(PLATFORM_OS)/
	mkdir bin/$(PLATFORM_OS)/Debug/
	mkdir obj/$(PLATFORM_OS)/Debug/
	mkdir lib/$(PLATFORM_OS)/Debug/
	mkdir bin/$(PLATFORM_OS)/Release/
	mkdir obj/$(PLATFORM_OS)/Release/
	mkdir lib/$(PLATFORM_OS)/Release/

clone:
	git clone https://github.com/glfw/glfw.git dep/glfw
	git clone https://github.com/PortAudio/portaudio.git dep/portaudio
	git clone https://github.com/assimp/assimp.git dep/assimp
	git clone https://gitlab.freedesktop.org/freetype/freetype.git dep/freetype
	git clone https://github.com/ShaderKitty/freeimage.git dep/freeimage
	(cd dep/glfw && git checkout 3.3.8)
	(cd dep/portaudio && git checkout v19.7.0)
	(cd dep/assimp && git checkout v5.2.5)
	(cd dep/freetype && git checkout VER-2-12-1)

builds:
	C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat amd64
	cmake -S dep/glfw/ -B dep/glfw/bld/$(PLATFORM_OS)/ -G $(PROJECT_TYPE)
	cmake -S dep/portaudio/ -B dep/portaudio/bld/$(PLATFORM_OS)/ -G $(PROJECT_TYPE) $(PORTAUDIO_CMAKE_OPTION)
	cmake -S dep/assimp/ -B dep/assimp/bld/$(PLATFORM_OS)/ -G $(PROJECT_TYPE) $(ASSIMP_CMAKE_OPTION)
	cmake -S dep/freetype/ -B dep/freetype/bld/$(PLATFORM_OS)/ -G $(PROJECT_TYPE) $(FREETYPE_CMAKE_OPTION)

dependencies:
	(cd dep/glfw/bld/$(PLATFORM_OS)/ && make)
	(cd dep/portaudio/bld/$(PLATFORM_OS)/ && make)
	(cd dep/assimp/bld/$(PLATFORM_OS)/ && make)
	(cd dep/freetype/bld/$(PLATFORM_OS)/ && make)
	(cd dep/freeimage/ && make)

# Compiles All Translation Units
$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLGS) -c $< $(INC) -o $@

# Links Library
geodesuka: $(OBJ)
	$(LIB) $(GEODESUKA_LIB) $(OBJ)

geodesuka-release: $(OBJ_REL)
	ar -crs $(GEODESUKA_LIB_REL) $(OBJ_REL)

unit-test-debug: geodesuka-debug
	$(CXX) $(FLG) -g -o $(UNIT_TEST_DEB) main.cpp $(INC) $(GEODESUKA_LIB_DEB) $(EXTERNAL_LIB)

unit-test-release: geodesuka-release
	$(CXX) $(FLG) -o $(UNIT_TEST_REL) main.cpp $(INC) $(GEODESUKA_LIB_REL) $(EXTERNAL_LIB)

clean-debug:
	rm $(UNIT_TEST_DEB)
	rm $(GEODESUKA_LIB_DEB)
	rm $(OBJ_DEB)

clean-release:
	rm $(UNIT_TEST_REL)
	rm $(GEODESUKA_LIB_REL)
	rm $(OBJ_REL)

clean-all: clean-debug clean-release	

