#version 430

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

out vec3 ViewPos;
out vec3 Normal;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

uniform float scale;

void main() 
{
    vec3 position = scale * vertex;
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0);
    ViewPos = (ViewMatrix * ModelMatrix * vec4(position, 1.0)).xyz;
    Normal = mat3(transpose(inverse(ModelMatrix))) * normal;
}