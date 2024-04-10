@echo off
setlocal
cd /d %~dp0

%VULKAN_SDK%\Bin\glslc.exe Shader.vert -o SPIRV/vert.spv
%VULKAN_SDK%\Bin\glslc.exe Shader.frag -o SPIRV/frag.spv