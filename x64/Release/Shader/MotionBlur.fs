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

const int smapleNum = 10;
const float threshold = 0.009;


void main() {
    float depth = texture(depthMap, TexCoords).z;

    vec4 h = vec4(TexCoords.x * 2 - 1, (1 - TexCoords.y) * 2 - 1, depth, 1);
    mat4 inv = inverse(projection * view);
    vec4 world_pos = inv * h;
    world_pos /= world_pos.w;

    vec4 currPos = h;
    vec4 lastPos = projection * lastView * world_pos;
    lastPos /= lastPos.w;

    depth = 2.0 * near * far / (far + near - (2.0 * (1 - depth) - 1.0) * (far - near)) + 8;
    vec2 velocity = (currPos - lastPos).xy / depth;


    fColor = texture(colorMap, TexCoords);
    if (length(velocity) > threshold) {
        velocity = velocity * (1 - exp(-length(velocity) * 10));
        vec2 texCoord = TexCoords;
        for (int i = 0; i < smapleNum; i++, texCoord += velocity) {
            fColor += texture(colorMap, texCoord);
        }
        fColor /= smapleNum + 1;
    }
}