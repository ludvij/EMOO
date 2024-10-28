//glsl version 4.5
#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require
#include "input_structures.glsl"


//shader input
layout (location = 0) in vec2 inTexCoords;
layout (location = 1) flat in int textureID;
//output write
layout (location = 0) out vec4 outFragColor;

//texture to access

void main() 
{
	// uncoment this when debugging
	// if (textureID < 0)
	// {
	// 	float scale = 8.0f;
	// 	int magenta = (int(inTexCoords.x * scale) % 2) ^ (int(inTexCoords.y * scale) % 2);
	// 	outFragColor = magenta == 1 ? vec4(1.0f, 0.0f, 1.0f, 1.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);
	// }
	// else 
	// {
	// }
	outFragColor = texture(displayTexture[textureID], inTexCoords);
}