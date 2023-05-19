
#GLFW = -lglfw -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl

# System Libraries
X11_LIB = -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl
ALSA_LIB = -lrt -lm -lasound
OPENGL_LIB = -lGL
VULKAN_LIB = -lvulkan
GLSLANG_LIB = -lGenericCodeGen -lglslang -lglslang-default-resource-limits -lHLSL -lMachineIndependent -lOGLCompiler -lOSDependent -lshaderc -lshaderc_combined
SPIRV_LIB = -lSPIRV -lspirv-cross-c -lspirv-cross-core -lspirv-cross-cpp -lspirv-cross-glsl -lspirv-cross-hlsl -lspirv-cross-msl -lspirv-cross-reflect -lspirv-cross-util -lSPIRV-Tools -lSPIRV-Tools-diff -lSPIRV-Tools-link -lSPIRV-Tools-lint -lSPIRV-Tools-opt -lSPIRV-Tools-reduce -lSPVRemapper

# Third Party 
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

CXX = g++
FLG = -std=c++17 -pthread
INC = -Iinc $(THIRDPARTY_INC)
LIB = $(THIRDPARTY_LIB) $(SYSTEM_LIB)
SRC = src/*.cpp
OBJ = obj/*.o

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

geodesuka-debug:
	$(CXX) $(FLG) -g -c $(SRC) $(INC) $(LIB)
	ar -crs geodesuka.a *.o
	mv *.o obj/Linux/Debug/
	mv geodesuka.a lib/Linux/Debug/

geodesuka-release:
	$(CXX) $(FLG) -c $(SRC) $(INC) $(LIB)
	ar -crs geodesuka.a *.o
	mv *.o obj/Linux/Release
	mv geodesuka.a lib/Linux/Release

unit-test-debug: geodesuka-debug
	$(CXX) $(FLG) -g -o bin/Linux/Debug/unit-test main.cpp $(INC) lib/Linux/Debug/geodesuka.a $(LIB)

unit-test-release: geodesuka-release
	$(CXX) $(FLG) -o bin/Linux/Release/unit-test main.cpp $(INC) lib/Linux/Debug/geodesuka.a $(LIB)
