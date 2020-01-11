#version 430 core

in vec4 clipSpace;
in vec2 textureCoords;

in vec3 vs_worldpos;

out vec4 fColor;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float near;
uniform float far;

uniform float moveFactor;

uniform vec3 color_specular;
uniform vec4 light_position;
uniform vec3 eye_position;
uniform float shininess = 64.0;
uniform float ambientStrength  = 0.4;
uniform float specularStrength = 0.5;
uniform vec3 Color = vec3(0.0, 0.0, 1.0);


const float waveStrength = 0.01;

void main(void) {
    vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;

    float depth = texture(depthMap, ndc).r;
    float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    depth = gl_FragCoord.z
    float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    float waterDepth = floorDistance - waterDistance;

    vec2 distortedTexCoords = texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 0.1;
	distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;


    ndc += totalDistortion;
    ndc = clamp(ndc, 0.001, 0.999);

    vec4 reflectionColor = texture(reflectionTexture, ndc);
    vec4 refractionColor = texture(refractionTexture, ndc);

    vec4 normalMapColor = texture(normalMap, distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
    normal = normalize(normal);

    vec3 eye_direction = normalize(eye_position - vs_worldpos);
    float reflectiveFactor = dot(eye_direction, normal);


    vec3 light_direction = normalize(light_position.xyz - vs_worldpos);
    vec3 half_vector = normalize(light_direction + eye_direction);
    float specular = pow(max(0.0, dot(normal, half_vector)), shininess);
    vec3 specular_highlight = color_specular * specular * specularStrength;

	fColor = mix(reflectionColor, refractionColor, reflectiveFactor);
	fColor = mix(fColor, vec4(0, 0.3, 0.5, 1.0), 0.2) + vec4(specular_highlight, 0.0);
    fColor = vec4(waterDepth / 50.0);
}