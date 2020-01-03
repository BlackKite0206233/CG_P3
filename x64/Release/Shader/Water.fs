#version 430 core

in vec2 textureCoords;

uniform vec3 color_ambient;
uniform vec3 color_diffuse;
uniform vec3 color_specular;
uniform vec4 light_position;
uniform vec3 eye_position;
uniform float shininess = 32.0;
uniform float ambientStrength  = 0.4;
uniform float specularStrength = 0.7;
uniform vec3 Color = vec3(0.0, 0.0, 1.0);

out vec4 fColor;

void main(void) {
	fColor = vec4(Color, 1.0);
}