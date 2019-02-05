#version 330

uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;
uniform vec2 lightPositionOnScreen;
uniform sampler2D firstPass;
uniform float numSamples;

out vec4 outColor;

void main() {	
    vec2 size = textureSize(firstPass, 0);
   	vec2 texCoord = gl_FragCoord.xy / size;

	vec2 deltaTextCoord = texCoord - lightPositionOnScreen;
	deltaTextCoord *= 1.0 / float(numSamples) * density;

    outColor = vec4(vec3(0.0), 1.0);
	float illuminationDecay = 1.0;
	for(int i = 0; i < numSamples ; ++i)
    {
        texCoord -= deltaTextCoord;

        vec4 sample = texture2D(firstPass, texCoord);
        sample *= illuminationDecay * weight;

        outColor += sample;

        illuminationDecay *= decay;
    }

    outColor *= exposure;
}