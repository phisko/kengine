namespace kengine::opengl::shaders::src {
	namespace SpotLight {
		namespace Frag {
			const char * glsl = R"(
#version 330

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D gdiffuse;
uniform sampler2D gspecular;

uniform vec3 viewPos;
uniform vec2 screenSize;

uniform vec4 color;
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

vec3 calcSpotLight(vec3 worldPos, vec3 normal, vec3 diffuse, vec3 specular) {
    vec3 viewDir = normalize(viewPos - worldPos);
    vec3 lightDir = normalize(position - worldPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    diffuse = diffuse * diffuseStrength * diff;

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    specular = specular * specularStrength * spec;

    // attenuation
    float dist = length(position - worldPos);
    float attenuation = 1.0 / (attenuationConstant + attenuationLinear * dist + attenuationQuadratic * (dist * dist));    

    // spotlight intensity
    float theta = dot(lightDir, normalize(-direction)); 
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    float shadow = calcShadow(worldPos, normal, lightDir);
    return color.rgb * (1.0 - shadow) * (diffuse + specular) * attenuation * intensity;
}

void main() {
   	vec2 texCoord = gl_FragCoord.xy / screenSize;

    vec4 diffuseAndShouldIgnoreLighting = texture(gdiffuse, texCoord);

    vec3 diffuse = diffuseAndShouldIgnoreLighting.rgb;
    float shouldIgnoreLighting = diffuseAndShouldIgnoreLighting.a;
    if (shouldIgnoreLighting == 1.0) {
        outputColor = vec4(diffuse, 1.0);
        return;
    }

   	vec3 worldPos = texture(gposition, texCoord).xyz;
   	vec3 normal = texture(gnormal, texCoord).xyz;
    vec3 specular = texture(gspecular, texCoord).rgb;

	outputColor = vec4(calcSpotLight(worldPos, normal, diffuse, specular), 1.0);
}
        )";
		}
	}
}