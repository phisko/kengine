#pragma once

struct BindFramebuffer {
	BindFramebuffer(GLuint fbo, GLuint prev = 0) : fbo(fbo), prev(prev) { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
	~BindFramebuffer() { glBindFramebuffer(GL_FRAMEBUFFER, prev); }

	GLuint fbo;
	GLuint prev;
};

struct Enable {
	Enable(GLenum feature) : feature(feature) { glEnable(feature); }
	~Enable() { glDisable(feature); }

	GLenum feature;
};

struct DepthMask {
	DepthMask() { glDepthMask(GL_TRUE); }
	~DepthMask() { glDepthMask(GL_FALSE); }
};