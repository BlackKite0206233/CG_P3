#version 430 core

layout (location = 0) in vec3 position;

out vec4 clipSpace;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

uniform float width;
uniform float height;

void main(void) {
    clipSpace = ProjectionMatrix * ViewMatrix * vec4(position.x * width, position.y, position.z * height, 1.0);
	gl_Position = clipSpace;
}