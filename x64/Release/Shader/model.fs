#version 430 core

in vec3 vs_worldpos;
in vec3 vs_normal;
in vec4 gl_FragCoord;
in vec4 clipSpace;

out vec4 fColor;

uniform vec3 color_ambient;
uniform vec3 color_diffuse;
uniform vec3 color_specular;
uniform vec4 light_position;
uniform vec3 eye_position;
uniform float shininess = 32.0;
uniform float ambientStrength  = 0.4;
uniform float specularStrength = 0.7;
uniform vec3 Color;

uniform int renderMode;
uniform sampler2D ssaoColorBufferBlur;

void main()
{
	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;

	float AmbientOcclusion = texture(ssaoColorBufferBlur, ndc).r;
	if (renderMode == 0) {
		AmbientOcclusion = 1;
	}

	vec3 light_direction = normalize(light_position.xyz - vs_worldpos);
	vec3 eye_direction = normalize(eye_position - vs_worldpos);
	vec3 normal = normalize(vs_normal);
	vec3 half_vector = normalize(light_direction + eye_direction);
	float diffuse = max(0.0, dot(normal, light_direction));
	float specular = pow(max(0.0, dot(normal, half_vector)), shininess);
	vec3 result = ambientStrength * color_ambient * AmbientOcclusion + diffuse * color_diffuse + specularStrength * specular * color_specular;
	vec3 color = Color;
	if (renderMode == 2) {
		color = vec4(AmbientOcclusion, AmbientOcclusion, AmbientOcclusion, 1);
	}
	fColor = min(vec4(color * result, 1), vec4(1));
}
