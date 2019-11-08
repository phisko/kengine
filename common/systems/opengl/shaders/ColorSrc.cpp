namespace kengine::Shaders::src {
    namespace Color {
		namespace Frag {
			const char * glsl = R"(
#version 330

uniform vec4 color;
out vec4 outColor;

void main() {
    outColor = color;
}
        )";
		}
    }
}