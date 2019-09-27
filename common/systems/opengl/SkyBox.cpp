#include "SkyBox.hpp"
#include "EntityManager.hpp"

#include "components/SkyBoxComponent.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"
#include "stb_image.h"

struct SkyBoxOpenGLComponent : kengine::not_serializable {
	GLuint textureID;
	pmeta_get_class_name(SkyBoxOpenGLComponent);
};

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

namespace kengine::Shaders {
	SkyBox::SkyBox(kengine::EntityManager & em)
		: Program(true, pmeta_nameof(SkyBox)),
		_em(em)
	{}

	void SkyBox::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<SkyBox>(putils::make_vector(
			ShaderDescription{ vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER }
		));

		_textureID = firstTextureID;
		putils::gl::setUniform(tex, _textureID);
	}

	static void loadSkyBox(Entity & e, const SkyBoxComponent & comp) {
		auto & skyBox = e.attach<SkyBoxOpenGLComponent>();

		if (skyBox.textureID == -1)
			glGenTextures(1, &skyBox.textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox.textureID);

		unsigned int i = 0;
		putils::for_each_attribute(SkyBoxComponent::get_attributes(), [&comp, &i](const char * name, auto member) {
			using MemberType = std::decay_t<decltype(SkyBoxComponent{}.*member) > ;
			if constexpr (std::is_same<MemberType, SkyBoxComponent::string>::value) {
				int width, height, nrChannels;
				const auto data = stbi_load((comp.*member).c_str(), &width, &height, &nrChannels, 0);
				assert(data != nullptr);
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
	}

	static void drawSkyBox() {
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

	void SkyBox::run(const Parameters & params) {
		use();

		ShaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		ShaderHelper::Enable depth(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		putils::gl::setUniform(view, glm::mat4(glm::mat3(params.view)));
		putils::gl::setUniform(proj, params.proj);
		putils::gl::setUniform(entityID, 0.f);

		glActiveTexture((GLenum)(GL_TEXTURE0 + _textureID));

		for (auto & [e, comp] : _em.getEntities<SkyBoxComponent>()) {
			putils::gl::setUniform(color, comp.color);

			if (!e.has<SkyBoxOpenGLComponent>())
				loadSkyBox(e, comp);
			glBindTexture(GL_TEXTURE_CUBE_MAP, e.get<SkyBoxOpenGLComponent>().textureID);
			drawSkyBox();
		}

		glDepthFunc(GL_LESS);
	}
}
