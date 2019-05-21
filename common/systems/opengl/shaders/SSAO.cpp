namespace kengine::Shaders::src {
    namespace SSAO {
        const char * frag = R"(
#version 330

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D noise;

uniform mat4 view;
uniform mat4 proj;
uniform vec2 screenSize;
uniform vec3 samples[64];

out float outColor;

const int KERNEL_SIZE = 64;
uniform float RADIUS;
uniform float FARCLIP;
uniform float BIAS;

void main() {
   	vec2 texCoord = gl_FragCoord.xy / screenSize;

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
        )";

        namespace Blur {
            const char * frag = R"(
#version 330

uniform sampler2D ssao;
uniform vec2 screenSize;

out float outColor;

void main() {
   	vec2 texCoord = gl_FragCoord.xy / screenSize;
    vec2 texelSize = 1.0 / screenSize;
    float result = 0.0;
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssao, texCoord + offset).r;
        }
    }

    outColor = result / (4.0 * 4.0);
}
            )";
        }
    }
}