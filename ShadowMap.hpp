#pragma once

#include <iostream>

#include <GL/glew.h>

class ShadowMap {
public:
	ShadowMap();
	~ShadowMap();

	virtual bool init(unsigned int _width, unsigned int _height);

	// when we want to write to the shadowmap for our first pass,
	// it will bind to the m_frameBufferObject to write to m_shadowMap
	virtual void bindToFrameBuffer();

	// When it comes to second pass, when rendering to screen (and use as a texture
	virtual void read(GLenum textureUnit);

	unsigned int getShadowWidth() { return m_width; }
	unsigned int getShadowHeight() { return m_height; }

	GLuint getShadowMapId() { return m_shadowMap; }

protected:
	GLuint
		m_frameBufferObject,
		m_shadowMap; // Id of a texture
		
	unsigned int 
		m_width,
		m_height;
};