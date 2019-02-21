#version 330

uniform sampler2D gposition;

uniform float SCATTERING;
uniform float NB_STEPS;
uniform float DEFAULT_STEP_LENGTH;
uniform float INTENSITY;

uniform mat4 inverseView;
uniform mat4 inverseProj;
uniform vec3 viewPos;

uniform vec3 color;
uniform vec3 direction;

out vec4 outColor;

#define PI 3.1415926535897932384626433832795

vec2 getShadowMapValue(vec3 worldPos);
vec3 getLightDirection(vec3 worldPos);

float computeScattering(float lightDotView) {
    float result = 1.0 - SCATTERING * SCATTERING;
    result /= (4.0 * PI * pow(1.0 + SCATTERING * SCATTERING - (2.0 * SCATTERING) * lightDotView, 1.5));
    return result;
}

void main() {	
    vec2 size = textureSize(gposition, 0);
   	vec2 texCoord = gl_FragCoord.xy / size;
    vec4 worldPos = texture(gposition, texCoord);

    vec3 lightDir = getLightDirection(worldPos.xyz);

    vec3 rayDir;
    float stepLength;
    if (worldPos.w == 0.0) { // "Empty" pixel, cast rays up to arbitrary distance
        vec2 normalizedScreenPos = texCoord * 2.0 - vec2(1.0, 1.0);
        vec4 pixelPos = inverseView * inverseProj * vec4(normalizedScreenPos.x, normalizedScreenPos.y, -1.0, 1.0);
        pixelPos.xyz *= pixelPos.w;
        rayDir = normalize(pixelPos.xyz - viewPos);
        stepLength = DEFAULT_STEP_LENGTH;
    }
    else { // "Full" pixel, cast rays up to it
        vec3 rayVector = worldPos.xyz - viewPos;
        rayDir = normalize(rayVector);
        stepLength = length(rayVector) / NB_STEPS;
    }

    vec3 rayStep = rayDir * stepLength;
    vec3 currentPos = viewPos;

    const mat4 ditherPattern = mat4(
        vec4(0.0f, 0.5f, 0.125f, 0.625f),
        vec4(0.75f, 0.22f, 0.875f, 0.375f),
        vec4(0.1875f, 0.6875f, 0.0625f, 0.5625),
        vec4(0.9375f, 0.4375f, 0.8125f, 0.3125)
    );
    float ditherValue = ditherPattern[int(gl_FragCoord.x) % 4][int(gl_FragCoord.y) % 4];
    currentPos += rayStep * ditherValue;

    vec3 accumFog = vec3(0.0);

    for (int i = 0; i < NB_STEPS; ++i) {
        vec2 shadow = getShadowMapValue(currentPos);
        if (shadow.x < 0.0 || shadow.x > shadow.y) {
            accumFog += computeScattering(dot(rayDir, lightDir)) * color * INTENSITY;
            currentPos += rayStep;
        }
    }
    accumFog /= NB_STEPS;

    outColor = vec4(accumFog, 1.0);
}