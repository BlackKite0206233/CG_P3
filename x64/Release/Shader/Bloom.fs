#version 430

in vec2 TexCoords;

out vec4 fColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lastView;

uniform sampler2D colorMap;
uniform sampler2D depthMap;

uniform float near;
uniform float far;

uniform float intensity;

const int smapleNum = 10;
const float threshold = 0.009;

float f(float x, float y){
    return exp(-0.5 * x * x * 0.05) * 0.5 * exp(-0.5 * y * y * 0.05) * 0.5;
}

void main() {
    fColor = texture(colorMap, TexCoords);
    float threshold = (100-intensity) * 0.01;
    for(int x = -8; x <= 8;x++ ){
        for(int y = -8; y <= 8;y++ ){
            vec4 color = texture(colorMap, TexCoords + vec2(x * 0.00135 * intensity, y * 0.001 * intensity));
            if(color.r > threshold && color.g > threshold && color.b > threshold){
                fColor.xyz += f(x,y) * color.xyz * 0.08;
            }
        }
    }
}