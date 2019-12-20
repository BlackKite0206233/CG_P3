#version 430 core

layout(location = 0) in vec3 vertex;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform vec3 Color;

out vec3 color;

void main(void)
{
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(vertex, 1.0);
    color = Color;
}
