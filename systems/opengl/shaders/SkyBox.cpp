#include "SkyBox.hpp"
#include "kengine.hpp"

#include "data/SkyBoxComponent.hpp"

#include "helpers/cameraHelper.hpp"
#include "helpers/assertHelper.hpp"
#include "shaderHelper.hpp"

#include "opengl/RAII.hpp"
#include "stb_image.h"

struct SkyBoxOpenGLComponent {
	putils::gl::Texture textureID;
};

#pragma region GLSL
static const auto vert = R"(
#version 330 core
layout (location = 0) in vec3 pos;

out vec3 TexCoords;

uniform mat4 proj;
uniform mat4 view;

void main() {
    TexCoords = pos;
    vec4 projectedPos = proj * view * vec4(pos, 1.0);
    gl_Position = projectedPos.xyww;
}  
)";

static const auto frag = R"(
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube tex;
uniform vec4 color;

void main() {    
    FragColor = texture(tex, TexCoords) * color;
}
)";
#pragma endregion GLSL

namespace kengine::opengl::shaders {
	SkyBox::SkyBox() noexcept
		: Program(true, putils_nameof(SkyBox))
	{}

	void SkyBox::init(size_t firstTextureID) noexcept {
		initWithShaders<SkyBox>(putils::make_vector(
			ShaderDescription{ vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER }
		));

		_textureID = firstTextureID;
		_tex = _textureID;
	}

	static SkyBoxOpenGLComponent & loadSkyBox(Entity & e, const SkyBoxComponent & comp) noexcept {
		auto & skyBox = e.attach<SkyBoxOpenGLComponent>();
		skyBox.textureID.generate();
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox.textureID);

		unsigned int i = 0;
		putils::reflection::for_each_attribute(comp, [&i](const char * name, auto && member) noexcept {
			if constexpr (std::is_same<putils_typeof(member), SkyBoxComponent::string>()) {
				int width, height, nrChannels;
				const auto data = stbi_load(member.c_str(), &width, &height, &nrChannels, 0);
				kengine_assert(data != nullptr);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
				++i;
			}
		});

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return skyBox;
	}

	static void drawSkyBox() noexcept {
		static GLuint vao = 0;
		static GLuint vbo;
		if (vao == 0) {
			const float vertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};

			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		}

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void SkyBox::run(const Parameters & params) noexcept {
		use();

		GLint fbo;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
		kengine_assert(fbo != 0);

		shaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		shaderHelper::Enable depth(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		_view = glm::mat4(glm::mat3(params.view));
		_proj = params.proj;
		_entityID = 0.f;

		glActiveTexture((GLenum)(GL_TEXTURE0 + _textureID));

		for (auto [e, comp] : entities.with<SkyBoxComponent>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			_color = comp.color;

			auto openGL = e.tryGet<SkyBoxOpenGLComponent>();
			if (!openGL)
				openGL = &loadSkyBox(e, comp);

			glBindTexture(GL_TEXTURE_CUBE_MAP, openGL->textureID);
			drawSkyBox();
		}

		glDepthFunc(GL_LESS);
	}
}
