#version 450
#extension GL_EXT_buffer_reference : require
#extension GL_GOOGLE_include_directive : require

#include "input_structures.glsl"

layout (location = 0) out vec2 outTexCoords;
layout (location = 1) flat out int textureID;

struct Vertex { 
	vec3 position;
	vec2 tex_coords;
	int textureID;
};  

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{	
	mat4 model;
	VertexBuffer vertexBuffer;
} PushConstants;

void main() 
{	
	//load vertex data from device adress
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	vec4 position = vec4(v.position, 1.0f);

	//output data
	gl_Position = sceneData.view * sceneData.proj * PushConstants.model * position;

	outTexCoords = v.tex_coords;
	textureID = v.textureID;
}
