#version 430 core

in vec3 vs_worldpos;
in vec3 vs_normal;
in vec2 pass_textureCoords;

out vec4 fColor;

uniform sampler2D grass;
uniform sampler2D mud;

uniform vec3 color_ambient;
uniform vec3 color_diffuse;
uniform vec3 color_specular;
uniform vec4 light_position;
uniform vec3 eye_position;
uniform float shininess = 2.0;
uniform float ambientStrength  = 0.4;
uniform float specularStrength = 0.4;

void main() {
    vec3 light_direction = normalize(light_position.xyz - vs_worldpos);
	vec3 eye_direction = normalize(eye_position - vs_worldpos);
	vec3 normal = normalize(vs_normal);
	vec3 half_vector = normalize(light_direction + eye_direction);
	float diffuse = max(0.0, dot(normal, light_direction));
	float specular = pow(max(0.0, dot(normal, half_vector)), shininess);
	vec3 result = ambientStrength * color_ambient + diffuse * color_diffuse + specularStrength * specular * color_specular;

	float blend = clamp(vs_worldpos.y, 0.0, 20.0) / 20.0;
	vec4 colorGrass = texture(grass, pass_textureCoords) * blend;
	vec4 colorMud = texture(mud, pass_textureCoords) * (1 - blend);
	vec3 color = (colorGrass + colorMud).rgb;
	if (vs_worldpos.y < 0) {
		color *= pow(1 + clamp(vs_worldpos.y, -100, 0) / 400, 8);
	}
	//color = vec3(0, 1, 0);
	fColor = min(vec4(color * result, 1), vec4(1));
}