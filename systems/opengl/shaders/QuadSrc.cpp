namespace kengine::opengl::shaders::src {
	namespace Quad {
		namespace Vert {
			const char * glsl = R"(
#version 330

layout (location = 0) in vec3 position;

void main() {
    gl_Position = vec4(position, 1.0);
}
        )";
		}
	}
}