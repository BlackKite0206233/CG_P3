#version 430 core
in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube day;
uniform samplerCube night;
uniform float blendFactor;

uniform vec3 fogColor;

const float lowerLimit = 0.0;
const float upperLimit = 0.3;

void main()
{    
    vec4 dayColor = texture(day, TexCoords);
    vec4 nightColor = texture(night, TexCoords);
    vec4 finalColor = mix(nightColor, dayColor, blendFactor);

    float factor = (TexCoords.y - lowerLimit) / (upperLimit - lowerLimit);
    factor = clamp(factor, 0.0, 1.0);

    FragColor = mix(vec4(fogColor, 1), finalColor, 1 - factor);
}