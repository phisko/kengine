#include <ft2build.h>
#include FT_FREETYPE_H

#include "Text.hpp"
#include "EntityManager.hpp"

#include "data/TextComponent.hpp"
#include "data/TransformComponent.hpp"

#include "ApplyTransparencySrc.hpp"

#include "ShaderHelper.hpp"

#include "helpers/CameraHelper.hpp"

#include "termcolor.hpp"
#include "magic_enum.hpp"

static FT_Library g_ft;
static GLuint g_vao, g_vbo;

static const auto vert = R"(
#version 330 core

layout (location = 0) in vec4 vertex; // vec2 pos, vec2 tex

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 viewPos;

out vec4 WorldPosition;
out vec3 EyeRelativePos;
out vec2 TexCoords;

void main() {
	WorldPosition = model * vec4(vertex.xy, 0.0, 1.0);
	EyeRelativePos = WorldPosition.xyz - viewPos;
    TexCoords = vertex.zw;

    gl_Position = proj * view * WorldPosition;
}
)";

static const auto frag = R"(
#version 330 core

uniform sampler2D tex;
uniform vec4 color;
uniform float entityID;

in vec4 WorldPosition;
in vec3 EyeRelativePos;
in vec2 TexCoords;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 gdiffuse;
layout (location = 3) out vec4 gspecular;
layout (location = 4) out float gentityID;

void applyTransparency(float alpha);

void main() {    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, TexCoords).r);
	sampled *= color;

	applyTransparency(sampled.a);

    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
    gdiffuse = vec4((sampled).xyz, 1.0); // don't apply lighting
	gspecular = vec4(0.0);
	gentityID = entityID;
}
)";

namespace kengine::Shaders {
	struct Font {
		struct Character {
			GLuint textureID;
			glm::ivec2 size;
			glm::ivec2 bearing;
			GLuint advance;
		};

		FT_Face face;
		std::unordered_map<unsigned long, Character> characters;

		auto createCharacter(unsigned long c) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cerr << putils::termcolor::red << "[FreeType] Error loading glyph `" << c << "`\n" << putils::termcolor::reset;
				return characters.end();
			}

			GLuint texture;

			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			const Character character{
				texture,
				{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
				{ face->glyph->bitmap_left, face->glyph->bitmap_top },
				(GLuint)face->glyph->advance.x
			};

			return characters.insert(std::make_pair(c, character)).first;
		}
	};

	struct FontSizes {
		std::unordered_map<size_t, Font> sizes;
	};

	static std::unordered_map<TextComponent::string, FontSizes> g_fonts;

	static auto createFont(const char * file, size_t size) {
		Font font;

		if (FT_New_Face(g_ft, file, 0, &font.face)) {
			std::cerr << putils::termcolor::red << "[FreeType] Error loading font `" << file << "`\n" << putils::termcolor::reset;
			return g_fonts[file].sizes.end();
		}

		if (FT_Set_Pixel_Sizes(font.face, 0, (FT_UInt)size)) {
			std::cerr << putils::termcolor::red << "[FreeType] Error setting size `" << size << "` for font `" << file << "`\n" << putils::termcolor::red;
			return g_fonts[file].sizes.end();
		}

		return g_fonts[file].sizes.insert(std::make_pair(size, std::move(font))).first;
	}

	Text::Text(EntityManager & em)
		: Program(false, putils_nameof(Text)),
		_em(em)
	{
		if (FT_Init_FreeType(&g_ft))
			assert(false && !"[FreeType] Could not init FreeType\n");
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	Text::~Text() {
		for (const auto &[file, font] : g_fonts)
			for (const auto &[size, fontSize] : font.sizes)
				FT_Done_Face(fontSize.face);
		FT_Done_FreeType(g_ft);
	}

	void Text::init(size_t firstTextureID) {
		initWithShaders<Text>(putils::make_vector(
			ShaderDescription{ vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ Shaders::src::ApplyTransparency::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_textureID = firstTextureID;
		_tex = _textureID;

		glGenVertexArrays(1, &g_vao);
		glGenBuffers(1, &g_vbo);
		glBindVertexArray(g_vao);
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	}

	struct Uniforms {
		size_t textureID;
		putils::gl::Uniform<putils::NormalizedColor> color;
		putils::gl::Uniform<glm::mat4> model;
	};

	static putils::Point2f getSizeAndGenerateCharacters(const TextComponent & text, Font & font, float scaleX, float scaleY) {
		putils::Point2f size;

		for (const auto c : text.text) {
			auto it = font.characters.find(c);
			if (it == font.characters.end()) {
				it = font.createCharacter(c);
				if (it == font.characters.end())
					continue;
			}

			const auto & character = it->second;
			size.x += (float)(character.advance >> 6) * scaleX;

			if (character.size.y > size.y)
				size.y = (float)character.size.y;
		}

		size.y *= scaleY;
		return size;
	}

	static void drawObject(const putils::gl::Program::Parameters & params, const TextComponent & text, const TransformComponent & transform, Uniforms uniforms, const glm::vec2 & screenSize, const TextComponent2D * comp = nullptr) {
		uniforms.color = text.color;

		auto & fontSizes = g_fonts[text.font];
		auto it = fontSizes.sizes.find(text.fontSize);
		if (it == fontSizes.sizes.end()) {
			it = createFont(text.font.c_str(), text.fontSize);
			if (it == fontSizes.sizes.end())
				return;
		}

		auto & font = it->second;

		auto scale = transform.boundingBox.size.y;
		if (comp != nullptr) {
			switch (comp->coordinateType) {
			case TextComponent2D::CoordinateType::Pixels:
				scale /= params.viewport.size.y;
				break;
			case TextComponent2D::CoordinateType::ScreenPercentage:
			default:
				static_assert(putils::magic_enum::enum_count<TextComponent2D::CoordinateType>() == 2);
				break;
			}
		}

		{
			const auto & box =
				comp == nullptr ?
				transform.boundingBox : // 3D
				CameraHelper::convertToScreenPercentage(transform.boundingBox, params.viewport.size, *comp); // 2D

			auto centre = ShaderHelper::toVec(box.position);

			glm::mat4 model(1.f);

			if (comp != nullptr) {
				centre.y = 1 - centre.y;
				model = glm::translate(model, glm::vec3(-1.f, -1.f, 0.f));
				centre *= 2.f;
			}

			model = glm::translate(model, centre);

			if (comp != nullptr)
				model = glm::scale(model, { scale, scale, scale });

			model = glm::rotate(model,
				transform.yaw,
				{ 0.f, 1.f, 0.f }
			);
			model = glm::rotate(model,
				transform.pitch,
				{ 1.f, 0.f, 0.f }
			);
			model = glm::rotate(model,
				transform.roll,
				{ 0.f, 0.f, 1.f }
			);

			if (comp == nullptr)
				model = glm::scale(model, { -scale, scale, scale });

			uniforms.model = model;
		}

		const auto scaleY = 1.f / (float)text.fontSize;
		const auto scaleX = scaleY * screenSize.y / screenSize.x;

		auto y = 0.f;
		auto x = 0.f; {
			const auto size = getSizeAndGenerateCharacters(text, font, scaleX, scaleY);
			y = -size.y / 2.f;

			switch (text.alignment) {
			case TextComponent::Alignment::Left:
				break;
			case TextComponent::Alignment::Center:
				x = -size.x / 2.f;
				break;
			case TextComponent::Alignment::Right:
				x = -size.x;
				break;
			default:
				static_assert(putils::magic_enum::enum_count<TextComponent::Alignment>() == 3);
				assert(false && !"Unknown text alignment");
			}
		}

		for (const auto c : text.text) {
			auto it = font.characters.find(c);
			if (it == font.characters.end())
				continue; // Characters were generated above, no need to try again

			const auto & character = it->second;

			glBindTexture(GL_TEXTURE_2D, character.textureID);

			const auto xpos = x + (float)character.bearing.x * scaleX;
			const auto ypos = y - (float)(character.size.y - character.bearing.y) * scaleY;

			const auto w = (float)character.size.x * scaleX;
			const auto h = (float)character.size.y * scaleY;

			const float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
			};

			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (float)(character.advance >> 6) * scaleX;
		}
	}

	void Text::run(const Parameters & params) {
		const Uniforms uniforms{ _textureID, _color, _model };

		use();

		_viewPos = params.camPos;
		glActiveTexture((GLenum)(GL_TEXTURE0 + uniforms.textureID));
		glBindVertexArray(g_vao);
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo);

		_view = glm::mat4(1.f);
		_proj = glm::mat4(1.f);
		for (const auto &[e, text, transform] : _em.getEntities<TextComponent2D, TransformComponent>()) {
			if (!ShaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			_entityID = (float)e.id;
			drawObject(params, text, transform, uniforms, glm::vec2(params.viewport.size.x, params.viewport.size.y), &text);
		}

		_view = params.view;
		_proj = params.proj;
		for (const auto &[e, text, transform] : _em.getEntities<TextComponent3D, TransformComponent>()) {
			if (!ShaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			_entityID = (float)e.id;
			drawObject(params, text, transform, uniforms, glm::vec2(params.viewport.size.x, params.viewport.size.y));
		}
	}
}