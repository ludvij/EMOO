//glsl version 4.5
#version 450

#extension GL_GOOGLE_include_directive : require
#include "input_structures.glsl"


//shader input
layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inTexCoords;
//output write
layout (location = 0) out vec4 outFragColor;

//texture to access

void main() 
{
	if (inTexCoords.x < 0 && inTexCoords.y < 0)
	{
		outFragColor = inColor;
	}
	else
	{
		outFragColor = texture(displayTexture, inTexCoords);
	}
}