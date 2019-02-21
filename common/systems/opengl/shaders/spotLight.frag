#version 330

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D gcolor;

uniform vec3 viewPos;

uniform vec3 color;
uniform vec3 position;
uniform vec3 direction;

uniform float cutOff;
uniform float outerCutOff;

uniform float diffuseStrength;
uniform float specularStrength;

uniform float attenuationConstant;
uniform float attenuationLinear;
uniform float attenuationQuadratic;

out vec4 outputColor;

float calcShadow(vec3 worldPos, vec3 normal, vec3 lightDir);

vec3 calcSpotLight(vec3 worldPos, vec3 normal) {
    vec3 viewDir = normalize(viewPos - worldPos);
    vec3 lightDir = normalize(position - worldPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // attenuation
    float dist = length(position - worldPos);
    float attenuation = 1.0 / (attenuationConstant + attenuationLinear * dist + attenuationQuadratic * (dist * dist));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-direction)); 
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    float diffuse = diffuseStrength * diff;
    float specular = specularStrength * spec;

    float shadow = calcShadow(worldPos, normal, lightDir);
    return color * (1.0 - shadow) * (diffuse + specular) * attenuation * intensity;
}

void main() {
   	vec2 texCoord = gl_FragCoord.xy / textureSize(gposition, 0);
   	vec3 worldPos = texture(gposition, texCoord).xyz;
   	vec3 objectColor = texture(gcolor, texCoord).xyz;
   	vec3 normal = texture(gnormal, texCoord).xyz;

   	outputColor = vec4(objectColor, 1.0) * vec4(calcSpotLight(worldPos, normal), 1.0);
}