#version 330

uniform sampler2D ssao;

out float outColor;

void main() {
   	vec2 texCoord = gl_FragCoord.xy / textureSize(ssao, 0);
    vec2 texelSize = 1.0 / textureSize(ssao, 0);
    float result = 0.0;
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssao, texCoord + offset).r;
        }
    }

    outColor = result / (4.0 * 4.0);
}