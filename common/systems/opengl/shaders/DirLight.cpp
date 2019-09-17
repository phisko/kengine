namespace kengine::Shaders::src {
    namespace DirLight {
        const char * frag = R"(
#version 330

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D gcolor;

uniform vec3 viewPos;
uniform vec2 screenSize;

uniform vec4 color;
uniform vec3 direction;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

out vec4 outputColor;

float calcShadow(vec3 worldPos, vec3 normal, vec3 lightDir);

vec3 calcDirLight(vec3 worldPos, vec3 normal) {
    vec3 viewDir = normalize(viewPos - worldPos);
    vec3 lightDir = normalize(-direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // combine results
    float ambient = ambientStrength; 
    float diffuse = diffuseStrength * diff;
    float specular = specularStrength * spec;

    float shadow = calcShadow(worldPos, normal, lightDir);
    return color.rgb * (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main() {
   	vec2 texCoord = gl_FragCoord.xy / screenSize;
   	vec3 worldPos = texture(gposition, texCoord).xyz;
   	vec4 objectColor = texture(gcolor, texCoord);
   	vec3 normal = texture(gnormal, texCoord).xyz;

	outputColor = vec4(objectColor.rgb, 1.0);
	if (objectColor.a == 0)
		outputColor = outputColor * vec4(calcDirLight(worldPos, normal), 1.0);
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