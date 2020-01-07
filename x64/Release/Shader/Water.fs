#version 430 core

in vec4 clipSpace;
in vec2 textureCoords;

in vec3 vs_worldpos;
in vec3 vs_normal;

out vec4 fColor;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;

uniform float moveFactor;

uniform vec3 color_ambient;
uniform vec3 color_diffuse;
uniform vec3 color_specular;
uniform vec4 light_position;
uniform vec3 eye_position;
uniform float shininess = 32.0;
uniform float ambientStrength  = 0.4;
uniform float specularStrength = 0.7;
uniform vec3 Color = vec3(0.0, 0.0, 1.0);


const float waveStrength = 0.01;

void main(void) {
    vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
    vec2 reflectionTexCoords = vec2(ndc.x, ndc.y);
    vec2 refractionTexCoords = vec2(ndc.x, ndc.y);

    vec2 distortion1 = (texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 2.0 - 1.0) * waveStrength;
    vec2 distortion2 = (texture(dudvMap, vec2(-textureCoords.x + moveFactor, textureCoords.y + moveFactor)).rg * 2.0 - 1.0) * waveStrength;
    vec2 totalDistortion = distortion1 + distortion2;

    reflectionTexCoords += totalDistortion;
    reflectionTexCoords = clamp(reflectionTexCoords, 0.001, 0.999);
    refractionTexCoords += totalDistortion;
    refractionTexCoords = clamp(refractionTexCoords, 0.001, 0.999);

    vec4 reflectionColor = texture(reflectionTexture, reflectionTexCoords);
    vec4 refractionColor = texture(refractionTexture, refractionTexCoords);

    vec3 eye_direction = normalize(eye_position - vs_worldpos);
    float reflectiveFactor = dot(eye_direction, vs_normal);

	fColor = mix(reflectionColor, refractionColor, reflectiveFactor);
	fColor = mix(fColor, vec4(0, 0.3, 0.5, 1.0), 0.2);
}