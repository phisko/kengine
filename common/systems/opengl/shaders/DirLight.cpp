namespace kengine::Shaders::src {
    namespace DirLight {
        const char * frag = R"(
#version 330

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D gcolor;
uniform sampler2D ssao;

uniform int runSSAO;

uniform vec3 viewPos;
uniform vec2 screenSize;

uniform vec3 color;
uniform vec3 direction;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

out vec4 outputColor;

float calcShadow(vec3 worldPos, vec3 normal, vec3 lightDir);

vec3 calcDirLight(vec3 worldPos, vec3 normal, float ambientOcclusion) {
    vec3 viewDir = normalize(viewPos - worldPos);
    vec3 lightDir = normalize(-direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // combine results
    float ambient = ambientStrength * ambientOcclusion;
    float diffuse = diffuseStrength * diff;
    float specular = specularStrength * spec;

    float shadow = calcShadow(worldPos, normal, lightDir);
    return color * (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main() {
   	vec2 texCoord = gl_FragCoord.xy / screenSize;
   	vec3 worldPos = texture(gposition, texCoord).xyz;
   	vec3 objectColor = texture(gcolor, texCoord).xyz;
   	vec3 normal = texture(gnormal, texCoord).xyz;
    float ambientOcclusion = texture(ssao, texCoord).r;
    if (runSSAO == 0)
        ambientOcclusion = 1.0;

   	outputColor = vec4(objectColor, 1.0) * vec4(calcDirLight(worldPos, normal, ambientOcclusion), 1.0);
}
        )";

        namespace GetDirection {
            const char * frag = R"(
#version 330

uniform vec3 direction;

vec3 getLightDirection(vec3 worldPos) {
    return -direction;
}
            )";
        }
    }
}