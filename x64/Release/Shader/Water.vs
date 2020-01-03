#version 430 core

layout (location = 0) in vec3 position;

out vec2 textureCoords;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

uniform float width;
uniform float height;

void main(void) {
	gl_Position = ProjectionMatrix * ViewMatrix * vec4(position.x * width, position.y, position.z * height, 1.0);
	textureCoords = vec2(position.x / 2.0 + 0.5, position.z / 2.0 + 0.5);
}