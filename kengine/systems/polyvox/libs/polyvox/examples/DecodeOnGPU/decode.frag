#version 130

// Passed in from the vertex shader
in vec4 worldPosition;
in vec4 worldNormal;

// the color that gets written to the display
out vec4 outputColor;

void main()
{
	// Again, for the purposes of these examples we cannot be sure that per-vertex normals are provided. A sensible fallback 
	// is to use this little trick to compute per-fragment flat-shaded normals from the world positions using derivative operations.
	//vec3 normal = normalize(cross(dFdy(worldPosition.xyz), dFdx(worldPosition.xyz)));
	
	// We are just using the normal as the output color, and making it lighter so it looks a bit nicer. 
	// Obviously a real shader would also do texuring, lighting, or whatever is required for the application.
	outputColor = vec4(abs(worldNormal.xyz) * 0.5 + vec3(0.5, 0.5, 0.5), 1.0);
}
