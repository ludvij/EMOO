layout(set = 0, binding = 0) uniform SceneData {
	mat4 view;
	mat4 proj;
} sceneData;

layout(set = 0, binding = 1) uniform sampler2D displayTexture;