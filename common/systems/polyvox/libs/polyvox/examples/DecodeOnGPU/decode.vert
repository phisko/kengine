#version 140

in uvec4 position; // This will be the position of the vertex in model-space
in uint normal;

// The usual matrices are provided
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

// This will be used by the fragment shader to calculate flat-shaded normals. This is an unconventional approach
// but we use it in this example framework because not all surface extractor generate surface normals.
out vec4 worldPosition;
out vec4 worldNormal;

// Returns +/- 1
vec2 signNotZero(vec2 v)
{
	return vec2((v.x >= 0.0) ? +1.0 : -1.0, (v.y >= 0.0) ? +1.0 : -1.0);
}

void main()
{
	vec4 decodedPosition = position;
	decodedPosition.xyz = decodedPosition.xyz * (1.0 / 256.0);
	
	//Get the encoded bytes of the normal
	uint encodedX = (normal >> 8u) & 0xFFu;
	uint encodedY = (normal) & 0xFFu;
	
	// Map to range [-1.0, +1.0]
	vec2 e = vec2(encodedX, encodedY);
	e = e * vec2(1.0 / 127.5, 1.0 / 127.5);
	e = e - vec2(1.0, 1.0);
	
	// Now decode normal using listing 2 of http://jcgt.org/published/0003/02/01/
	vec3 v = vec3(e.xy, 1.0 - abs(e.x) - abs(e.y));
	if (v.z < 0) v.xy = (1.0 - abs(v.yx)) * signNotZero(v.xy);	
	worldNormal.xyz = normalize(v);
	worldNormal.w = 1.0;
	
	// Standard sequence of OpenGL transformations.
	worldPosition = modelMatrix * decodedPosition;
	vec4 cameraPosition = viewMatrix * worldPosition;
	gl_Position = projectionMatrix * cameraPosition;
}
