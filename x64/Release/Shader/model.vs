#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

out vec3 vs_worldpos;
out vec3 vs_normal;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform float Scale;

uniform vec4 clipPlane;

void main(void)
{
    vec4 position = ModelMatrix * vec4(Scale * vertex, 1.0);

    gl_ClipDistance[0] = dot(position, clipPlane);

    gl_Position = ProjectionMatrix * ViewMatrix * position;
    vs_worldpos = position.xyz;
    vs_normal = mat3(transpose(inverse(ModelMatrix))) * normal;
}
