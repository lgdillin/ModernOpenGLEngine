#pragma once
// OmniShadowMap as it points in any direction

#include "ShadowMap.hpp"

class OmniShadowMap : public ShadowMap {
public:
	OmniShadowMap();
	~OmniShadowMap();

	bool init(unsigned int _width, unsigned int _height);

	// when we want to write to the shadowmap for our first pass,
	// it will bind to the m_frameBufferObject to write to m_shadowMap
	void bindToFrameBuffer();

	// When it comes to second pass, when rendering to screen (and use as a texture
	void read(GLenum textureUnit);

private:

};