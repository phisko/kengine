#pragma once

#include <vector>
#include <GL/glew.h>
#include <GL/GL.h>
#include "Point.hpp"
#include "packets/GBuffer.hpp"

namespace kengine {
	class GBufferComponent {
	public:
		void init(size_t width, size_t height, size_t nbAttributes) {
			if (!textures.empty())
				return; // Already init

			_size = { (unsigned int)width, (unsigned int)height };

			textures.resize(nbAttributes);

			std::vector<GLenum> attachments;

			glGenFramebuffers(1, &_fbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);

			glGenTextures((GLsizei)textures.size(), textures.data());
			glGenTextures(1, &_depthTexture);

			for (size_t i = 0; i < textures.size(); ++i) {
				glBindTexture(GL_TEXTURE_2D, textures[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_FLOAT, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				const auto attachmentNumber = GL_COLOR_ATTACHMENT0 + i;
				glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)attachmentNumber, GL_TEXTURE_2D, textures[i], 0);
				attachments.push_back((GLenum)attachmentNumber);
			}

			glBindTexture(GL_TEXTURE_2D, _depthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (GLsizei)width, (GLsizei)height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0);

			glDrawBuffers((GLsizei)attachments.size(), attachments.data());

			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		void resize(size_t screenWidth, size_t screenHeight) {
			_size = { (unsigned int)screenWidth, (unsigned int)screenHeight };

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);

			for (const auto texture : textures) {
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei)screenWidth, (GLsizei)screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
			}

			glBindTexture(GL_TEXTURE_2D, _depthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (GLsizei)screenWidth, (GLsizei)screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		}

		void bindForWriting() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		}

		void bindForReading() {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);

			for (unsigned int i = 0; i < textures.size(); ++i) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, textures[i]);
			}
		}

		auto getTextureCount() const { return textures.size(); }
		auto getFBO() const { return _fbo; }

		const auto & getSize() const { return _size; }

		void getTexture(size_t textureIndex, float * buff, size_t buffSize) const {
			if (glGetnTexImage == nullptr) {
				memset(buff, 0, buffSize * sizeof(float));
				return;
			}
			else {
				glBindTexture(GL_TEXTURE_2D, textures[textureIndex]);
				glGetnTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, (GLsizei)buffSize * sizeof(float), buff);
			}
		}

		std::vector<GLuint> textures;

		bool isInit() const { return !textures.empty(); }

	private:
		GLuint _fbo;
		GLuint _depthTexture;
		putils::Point2ui _size;
	};
}
