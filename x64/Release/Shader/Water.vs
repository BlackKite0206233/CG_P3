#version 430 core

layout (location = 0) in vec3 position;

out vec4 clipSpace;
out vec2 textureCoords;

out vec3 vs_worldpos;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

uniform float width;
uniform float height;

const float tiling = 5.0;

void main(void) {
    vs_worldpos = vec3(position.x * width, position.y, position.z * height);
    clipSpace = ProjectionMatrix * ViewMatrix * vec4(vs_worldpos, 1.0);
	gl_Position = clipSpace;
    textureCoords = vec2(position.x / 2.0 + 0.5, position.z / 2.0 + 0.5) * tiling;
}