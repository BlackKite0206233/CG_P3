#version 430 core

in vec4 clipSpace;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

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
    vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
    vec2 reflectionTexCoords = vec2(ndc.x, ndc.y);
    vec2 refractionTexCoords = vec2(ndc.x, ndc.y);
    vec4 reflectionColor = texture(reflectionTexture, reflectionTexCoords);
    vec4 refractionColor = texture(refractionTexture, refractionTexCoords);

	fColor = mix(reflectionColor, refractionColor, 0.5);
	//fColor = vec4(0, 0, 1, 0);
}