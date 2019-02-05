#version 330

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D noise;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 samples[64];

out float outColor;

const int KERNEL_SIZE = 64;
uniform float RADIUS;
uniform float FARCLIP;
uniform float BIAS;

void main() {
    vec2 size = textureSize(gposition, 0);
   	vec2 texCoord = gl_FragCoord.xy / size;

    vec3 worldPos = (proj * view * texture(gposition, texCoord)).xyz;
    float depth = worldPos.z * FARCLIP;

    vec3 normal = normalize((proj * view * texture(gnormal, texCoord)).xyz);
    float scale = RADIUS / depth;

    float occlusion = 0.0;
    for (int i = 0; i < KERNEL_SIZE; ++i) {
        vec3 randNor = normalize(samples[i]);
        if (dot(normal, randNor) < 0.0)
            randNor *= -1.0;

        vec2 off = randNor.xy * scale;
        float sampleDepth = (proj * view * texture(gposition, texCoord + off)).z;
        float depthDelta = depth - sampleDepth * FARCLIP;

        vec3 sampleDir = vec3(randNor.xy * RADIUS, depthDelta);
        float occ = max(0.0, dot(normalize(normal), normalize(sampleDir)) - BIAS) / (length(sampleDir) + 1.0);
        occlusion += 1.0 - occ;
    }
    occlusion /= float(KERNEL_SIZE);

    outColor = occlusion;
}