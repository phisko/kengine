namespace kengine::Shaders::src {
    namespace Color {
        const char * frag = R"(
#version 330

uniform vec4 color;
out vec4 outColor;

void main() {
    outColor = color;
}
        )";
    }
}