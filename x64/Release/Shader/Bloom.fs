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

uniform float width;
uniform float height;

uniform float intensity;

const int smapleNum = 10;
const float threshold = 0.009;

const vec2 invImageSize = vec2(1 / 1920.0, 1 / 1080.0);

float f(float x, float y) {
    return exp(-(x * x + y * y));
}

void main() {
    fColor = texture(colorMap, TexCoords);
    float threshold = (100 - intensity) * 0.01;
    float step = intensity * 0.1;
    float clampStep = clamp(step, 1, 10);
    for (int x = -8 - int(step); x <= 8 + int(step); x++){
        for (int y = -8 - int(step); y <= 8 + int(step); y++){
            vec4 color = texture(colorMap, TexCoords + vec2(x / width * clampStep, y / height * clampStep));
            if (color.r > threshold && color.g > threshold && color.b > threshold){
                fColor.xyz += f(x,y) * color.xyz * 0.08;
            }
        }
    }
}