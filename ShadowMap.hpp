#pragma once

#include <iostream>

#include <GL/glew.h>

class ShadowMap {
public:
	ShadowMap();
	~ShadowMap();

	virtual bool init(GLuint _width, GLuint _height);

	// when we want to write to the shadowmap for our first pass,
	// it will bind to the m_frameBufferObject to write to m_shadowMap
	virtual void write();

	// When it comes to second pass, when rendering to screen (and use as a texture
	virtual void read(GLenum textureUnit);

	GLuint getShadowWidth() { return m_width; }
	GLuint getShadowHeight() { return m_height; }

protected:
	GLuint
		m_frameBufferObject,
		m_shadowMap, // Id of a texture
		m_width,
		m_height;
};