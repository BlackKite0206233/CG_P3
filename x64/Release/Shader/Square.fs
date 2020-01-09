#version 430 core

in vec2 vUV;

out vec4 fColor;

uniform sampler2D Texture;

void main()
{
	
	fColor = texture2D(Texture,vUV);
}
