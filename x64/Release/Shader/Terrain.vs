#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;

out vec3 vs_worldpos;
out vec3 vs_normal;
out vec2 pass_textureCoords;
out vec4 clipSpace;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

uniform vec4 clipPlane;

void main(void)
{
    vec4 position = vec4(vertex, 1.0);

    gl_ClipDistance[0] = dot(position, clipPlane);

    clipSpace = ProjectionMatrix * ViewMatrix * position;
    gl_Position = clipSpace;
    vs_worldpos = position.xyz;
    vs_normal = normal;
    pass_textureCoords = textureCoords / 4.0;
}
