#version 430 core

layout(location = 0) in vec3 vertex;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec3 Color;

out vec3 color;

void main(void)
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex, 1.0);
    color = Color;
}
