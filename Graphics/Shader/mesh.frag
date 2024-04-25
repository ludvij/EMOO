//glsl version 4.5
#version 450

#extension GL_GOOGLE_include_directive : require
#include "input_structures.glsl"


//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;
//output write
layout (location = 0) out vec4 outFragColor;

//texture to access

void main() 
{
	outFragColor = texture(displayTexture,inUV);
}