#version 430 core

in vec3 vs_worldpos;
in vec3 vs_normal;
in vec2 pass_textureCoords;
in vec4 clipSpace;
in float visibility;

out vec4 fColor;

uniform sampler2D grass;
uniform sampler2D mud;
uniform sampler2D water;

uniform int renderMode;
uniform sampler2D ssaoColorBufferBlur;

uniform vec3 color_ambient;
uniform vec3 color_diffuse;
uniform vec3 color_specular;
uniform vec4 light_position;
uniform vec3 eye_position;
uniform float shininess = 2.0;
uniform float ambientStrength  = 0.4;
uniform float specularStrength = 0.4;
uniform vec3 fogColor;

void main() {
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
	if (renderMode == 2) {
		float level = 4.0;
		float q = 1.0 / level;
		diffuse = pow(diffuse, 5.0);
		diffuse = float(int(diffuse / q)) * q + q / 2.0;
		result = diffuse * color_diffuse;
	}

	float blend = clamp(vs_worldpos.y, 0.0, 20.0) / 20.0;
	vec4 colorGrass = texture(grass, pass_textureCoords) * blend;
	vec4 colorMud = texture(mud, pass_textureCoords) * (1 - blend);
	float blend2 = clamp(vs_worldpos.y, -100.0, 0.0) / -100.0;
	vec4 colorWater = texture(water, pass_textureCoords) * blend2;
	vec3 color = (colorGrass + colorMud).rgb * (1 - blend2);
	color = (color + colorWater.rgb);
	if (renderMode == 2) {
		color = vec3(AmbientOcclusion, AmbientOcclusion, AmbientOcclusion);
	}
	if (vs_worldpos.y < 0) {
		color *= pow(1 + clamp(vs_worldpos.y, -100, 0) / 400, 8);
	}
	fColor = min(vec4(color * result, 1), vec4(1));
	fColor = mix(vec4(fogColor, 1), fColor, visibility);
}