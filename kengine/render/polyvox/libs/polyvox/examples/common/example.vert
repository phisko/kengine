#version 140

in vec4 position; // This will be the position of the vertex in model-space

// The usual matrices are provided
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

// This will be used by the fragment shader to calculate flat-shaded normals. This is an unconventional approach
// but we use it in this example framework because not all surface extractor generate surface normals.
out vec4 worldPosition;

void main()
{
	// Standard sequence of OpenGL transformations.
	worldPosition = modelMatrix * position;
	vec4 cameraPosition = viewMatrix * worldPosition;
	gl_Position = projectionMatrix * cameraPosition;
}
