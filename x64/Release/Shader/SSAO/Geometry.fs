#version 430

in vec3 ViewPos;
in vec3 Normal;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main() 
{
    gPosition = vec4(ViewPos, 1.0);
    gNormal = normalize(Normal);
    gAlbedoSpec = vec4(vec3(0.95), 0.0);
}