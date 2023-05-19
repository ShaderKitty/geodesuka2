# Installs proper dependencies

# GLFW System Window dependencies
# https://www.glfw.org/docs/3.3/compile.html

#Install unzip
sudo apt-get install zip unzip

# Install X11 SDK
sudo apt install xorg-dev
# -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl

# Install Wayland SDK
#sudo apt install libwayland-dev libxkbcommon-dev wayland-protocols extra-cmake-modules

# Install Alsa SDK
sudo apt-get install libasound-dev
# -lrt -lm -lasound -ljack -pthread

# Install Vulkan SDK
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.236-jammy.list https://packages.lunarg.com/vulkan/1.3.236/lunarg-vulkan-1.3.236-jammy.list
sudo apt update
sudo apt install vulkan-sdk
# VULKAN: -lvulkan
# GLSLANG: -lGenericCodeGen -lglslang -lglslang-default-resource-limits -lHLSL -lMachineIndependent -lOGLCompiler -lOSDependent -lshaderc -lshaderc_combined
# SPIRV: -lSPIRV -lspirv-cross-c -lspirv-cross-core -lspirv-cross-cpp -lspirv-cross-glsl -lspirv-cross-hlsl -lspirv-cross-msl -lspirv-cross-reflect -lspirv-cross-util -lSPIRV-Tools -lSPIRV-Tools-diff -lSPIRV-Tools-link -lSPIRV-Tools-lint -lSPIRV-Tools-opt -lSPIRV-Tools-reduce -lSPVRemapper