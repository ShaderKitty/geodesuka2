# GLFW = -lglfw -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl



ifeq ($(OS),Windows_NT)
	PLATFORM_OS = Windows
	CXX = cl
	CCFLAGS = -std=c++17 -pthread
    CCFLAGS += -D WIN32
    #ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
    #    CCFLAGS += -D AMD64
    #else
    #    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
    #        CCFLAGS += -D AMD64
    #    endif
    #    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
    #        CCFLAGS += -D IA32
    #    endif
    #endif

	DEB_DIR := Windows/Debug
	REL_DIR := Windows/Release
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		PLATFORM_OS = Linux
		CCFLAGS += -D LINUX

		X11_LIB = -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl
		ALSA_LIB = -lrt -lm -lasound
		OPENGL_LIB = -lGL
		VULKAN_LIB = -lvulkan
		GLSLANG_LIB = -lGenericCodeGen -lglslang -lglslang-default-resource-limits -lHLSL -lMachineIndependent -lOGLCompiler -lOSDependent -lshaderc -lshaderc_combined
		SPIRV_LIB = -lSPIRV -lspirv-cross-c -lspirv-cross-core -lspirv-cross-cpp -lspirv-cross-glsl -lspirv-cross-hlsl -lspirv-cross-msl -lspirv-cross-reflect -lspirv-cross-util -lSPIRV-Tools -lSPIRV-Tools-diff -lSPIRV-Tools-link -lSPIRV-Tools-lint -lSPIRV-Tools-opt -lSPIRV-Tools-reduce -lSPVRemapper

		GLFW_INC = -Idep/glfw/include/
		PORTAUDIO_INC = -Idep/portaudio/include/
		ASSIMP_INC = -Idep/assimp/bld/linux/include/ -Idep/assimp/include/
		FREETYPE_INC = -Idep/freetype/bld/linux/include/ -Idep/freetype/include/
		FREEIMAGE_INC = -Idep/freeimage/Source/

		GLFW_LIB = dep/glfw/bld/linux/src/libglfw3.a
		PORTAUDIO_LIB = dep/portaudio/bld/linux/libportaudio.a
		ASSIMP_LIB = dep/assimp/bld/linux/lib/libassimp.a
		FREETYPE_LIB = dep/freetype/bld/linux/libfreetype.a
		FREEIMAGE_LIB = dep/freeimage/libfreeimage.a

		SYSTEM_LIB = $(ALSA_LIB) $(X11_LIB) $(GLSLANG_LIB) $(SPIRV_LIB) $(VULKAN_LIB) $(OPENGL_LIB)

		THIRDPARTY_INC = $(FREEIMAGE_INC) $(FREETYPE_INC) $(ASSIMP_INC) $(PORTAUDIO_INC) $(GLFW_INC)
		THIRDPARTY_LIB = $(FREEIMAGE_LIB) $(FREETYPE_LIB) $(ASSIMP_LIB) $(PORTAUDIO_LIB) $(GLFW_LIB)
    endif
    ifeq ($(UNAME_S),Darwin)
		PLATFORM_OS = MacOS
        CCFLAGS += -D OSX
    endif
    #UNAME_P := $(shell uname -p)
    #ifeq ($(UNAME_P),x86_64)
    #    CCFLAGS += -D AMD64
    #endif
    #ifneq ($(filter %86,$(UNAME_P)),)
    #    CCFLAGS += -D IA32
    #endif
    #ifneq ($(filter arm%,$(UNAME_P)),)
    #    CCFLAGS += -D ARM
    #endif

endif

DEB_DIR := $(PLATFORM_OS)/Debug
REL_DIR := $(PLATFORM_OS)/Release

CXX = g++
FLG = -std=c++17 -pthread
INC = -Iinc $(THIRDPARTY_INC)
EXTERNAL_LIB = $(THIRDPARTY_LIB) $(SYSTEM_LIB)

# Source Paths
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.cpp)

# Generate Directory strings for object compilation.
OBJ_DEB_DIR := obj/$(DEB_DIR)
OBJ_REL_DIR := obj/$(REL_DIR)

# Path to geodesuka output library.
GEODESUKA_LIB_DEB := lib/$(DEB_DIR)/geodesuka.a
GEODESUKA_LIB_REL := lib/$(REL_DIR)/geodesuka.a

# Path to unit tests
UNIT_TEST_DEB := bin/$(DEB_DIR)/unit-test
UNIT_TEST_REL := bin/$(REL_DIR)/unit-test

# Path substitution for all compiled objects.
OBJ_DEB := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DEB_DIR)/%.o,$(SRC))
OBJ_REL := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_REL_DIR)/%.o,$(SRC))

$(OBJ_DEB): $(OBJ_DEB_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(FLG) -g -c $< $(INC) -o $@

$(OBJ_REL): $(OBJ_REL_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(FLG) -c $< $(INC) -o $@

dirs: 
	mkdir bin/
	mkdir obj/
	mkdir lib/
	mkdir bin/Linux/
	mkdir obj/Linux/
	mkdir lib/Linux/
	mkdir bin/Linux/Debug/
	mkdir obj/Linux/Debug/
	mkdir lib/Linux/Debug/
	mkdir bin/Linux/Release/
	mkdir obj/Linux/Release/
	mkdir lib/Linux/Release/

dependencies:
	(cd dep/glfw/bld/linux/ && make)
	(cd dep/portaudio/bld/linux/ && make)
	(cd dep/assimp/bld/linux/ && make)
	(cd dep/freetype/bld/linux/ && make)
	(cd dep/freeimage/ && make)

geodesuka-debug: $(OBJ_DEB)
	ar -crs $(GEODESUKA_LIB_DEB) $(OBJ_DEB)

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

