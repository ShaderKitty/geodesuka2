# make -e ARCH=x64 MODE=Release [RECIPE]

# Determines Compiler Info
ifeq ($(COMPILER),gcc)
	LNK = ld
	LIB = ar -rcs
	CFLGS = -std=c++17 -pthread
	LFLGS = -std=c++17 -pthread
	ifeq ($(MODE),Debug)
		CFLGS += -g
		LFLGS += -g
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

# Determines Platform Operating System
ifeq ($(OS),Windows_NT)
	PLAT = Windows
	PROJECT_TYPE = "NMake Makefiles"
	MAKE_TOOL = nmake
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		PLAT = Linux
	endif
    ifeq ($(UNAME_S),Darwin)
		PLAT = MacOS
    endif
	PROJECT_TYPE = "Unix Makefiles"
	MAKE_TOOL = make
endif

PLAT = Linux
PROJECT_TYPE = "Unix Makefiles"

# This is used to discriminate between compile types.
DISC = $(PLAT)/$(ARCH)/$(MODE)

GLM_INC = -Idep/glm/
GLFW_INC = -Idep/glfw/include/
PORTAUDIO_INC = -Idep/portaudio/include/
ASSIMP_INC = -Idep/assimp/include/ -Idep/assimp/bld/$(DISC)/include/
FREETYPE_INC = -Idep/freetype/include/ -Idep/freetype/bld/$(DISC)/include/
FREEIMAGE_INC = -Idep/freeimage/Source/

GLFW_LIB = -Ldep/glfw/bld/$(DISC)/src -lglfw3
PORTAUDIO_LIB = -Ldep/portaudio/bld/$(DISC) -lportaudio
ASSIMP_LIB = -Ldep/assimp/bld/$(DISC)/lib -lassimp
FREETYPE_LIB = -Ldep/bld/freetype/$(DISC) -lfreetype
FREEIMAGE_LIB = -Ldep/freeimage/Dist -lfreeimage

X11_LIB = \
-lXrandr \
-lXxf86vm \
-lXi \
-lXinerama \
-lX11 \
-ldl \
-lpthread

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

GEODESUKA = lib/$(DISC)/geodesuka.a

DEP_DIR = dep

SYS_INC = $(VULKAN_INC)
SYS_LIB = $(GLSLANG_LIB) $(SPIRV_LIB) $(VULKAN_LIB) $(OPENGL_LIB) $(ALSA_LIB) $(X11_LIB) 

DEP_INC = $(FREEIMAGE_INC) $(FREETYPE_INC) $(ASSIMP_INC) $(PORTAUDIO_INC) $(GLFW_INC) $(GLM_INC)
DEP_LIB = $(FREEIMAGE_LIB) $(FREETYPE_LIB) $(ASSIMP_LIB) $(PORTAUDIO_LIB) $(GLFW_LIB)

INC = -Iinc $(DEP_INC) $(SYS_INC)
LIB = $(DEP_LIB) $(SYS_LIB)

# Source Paths
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.cpp)

# Path substitution for all compiled objects.
OBJ_DIR := obj/$(DISC)
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

# Installs system dependencies
install:
	sudo apt-get install zip unzip
	sudo apt install xorg-dev
	sudo apt-get install libasound-dev
	wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
	sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.250-jammy.list https://packages.lunarg.com/vulkan/1.3.250/lunarg-vulkan-1.3.250-jammy.list
	sudo apt update
	sudo apt install vulkan-sdk

# Clones third party library depdendencies
clone:
	git clone https://github.com/glfw/glfw.git $(DEP_DIR)/glfw
	git clone https://github.com/PortAudio/portaudio.git $(DEP_DIR)/portaudio
	git clone https://github.com/assimp/assimp.git dep/assimp
	git clone https://gitlab.freedesktop.org/freetype/freetype.git $(DEP_DIR)/freetype
	git clone https://github.com/ShaderKitty/freeimage.git $(DEP_DIR)/freeimage
	git clone https://github.com/g-truc/glm.git $(DEP_DIR)/glm
	(cd $(DEP_DIR)/glfw && git checkout 3.3.8)
	(cd $(DEP_DIR)/portaudio && git checkout v19.7.0)
	(cd $(DEP_DIR)/assimp && git checkout v5.2.5)
	(cd $(DEP_DIR)/freetype && git checkout VER-2-12-1)
	(cd $(DEP_DIR)/glm && git checkout 0.9.9.8)

# Setup build files
setup:
	cmake -S $(DEP_DIR)/glfw/ -B $(DEP_DIR)/glfw/bld/$(DISC)/ -G $(PROJECT_TYPE)
	cmake -S $(DEP_DIR)/portaudio/ -B $(DEP_DIR)/portaudio/bld/$(DISC)/ -G $(PROJECT_TYPE) -D PA_BUILD_SHARED=OFF
	cmake -S $(DEP_DIR)/assimp/ -B $(DEP_DIR)/assimp/bld/$(DISC)/ -G $(PROJECT_TYPE) -D BUILD_SHARED_LIBS=OFF -D ASSIMP_BUILD_TESTS=OFF -D ASSIMP_BUILD_ZLIB=ON
	cmake -S $(DEP_DIR)/freetype/ -B $(DEP_DIR)/freetype/bld/$(DISC)/ -G $(PROJECT_TYPE) -D FT_DISABLE_ZLIB=TRUE -D FT_REQUIRE_ZLIB=FALSE
	sed -i 's/#define FT_CONFIG_OPTION_USE_BROTLI/\/\/#define FT_CONFIG_OPTION_USE_BROTLI/' dep/freetype/bld/$(DISC)/include/freetype/config/ftoption.h

# Compile Dependencies
dependencies:
	(cd dep/glfw/bld/$(DISC)/ && make)
	(cd dep/portaudio/bld/$(DISC)/ && make)
	(cd dep/assimp/bld/$(DISC)/ && make)
	(cd dep/freetype/bld/$(DISC)/ && make)
	(cd dep/freeimage/ && make)

# Makes output dirs for geodesuka
dirs: 
	mkdir -p bin/$(DISC)
	mkdir -p obj/$(DISC)
	mkdir -p lib/$(DISC)

# Compiles All Translation Units
$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLGS) -c $< $(INC) -o $@

# Creates library
geodesuka: $(OBJ)
	ar -crs $(GEODESUKA) $(OBJ)

# Compiles unit test
unit-test: geodesuka
	$(CXX) $(LFLGS) -o bin/$(DISC)/$@ main.cpp $(INC) $(GEODESUKA) $(LIB)

# Cleans cached object files
clean:
	rm bin/$(DISC)/*
	rm lib/$(DISC)/*.a
	rm obj/$(DISC)/*.o
