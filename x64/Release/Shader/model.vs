#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform float Scale;

out vec3 vs_worldpos;
out vec3 vs_normal;

void main(void)
{
    vec4 position = ModelMatrix * vec4(Scale * vertex, 1.0);

    gl_Position = ProjectionMatrix * ViewMatrix * position;
    vs_worldpos = position.xyz;
    vs_normal = mat3(transpose(inverse(ModelMatrix))) * normal;
}
