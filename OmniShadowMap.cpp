#include "OmniShadowMap.hpp"

OmniShadowMap::OmniShadowMap() : ShadowMap() {
}

OmniShadowMap::~OmniShadowMap() {
}

bool OmniShadowMap::init(unsigned int _width, unsigned int _height) {
	m_width = _width;
	m_height = _height;

	glGenFramebuffers(1, &m_frameBufferObject);
	glGenTextures(1, &m_shadowMap);

	// instead of using a texture, we are using a cubemap
	// its not a 2D texture, its 6 textures 'folded' to make a cube
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);

	// Using a geometry shader between vertex and frag shader. (?)
	// go through each of the 6 textures inside our cube map
	for (int i = 0; i < 6; ++i) {
		// cycle through each cube map size
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F,
			m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}

	// Manage the scaling of the texture's look when very close/far
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Handle texture wrapping on the S and T axis (read: X and Y axis)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Consider this the 'Z' directon (depth)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject);
	// have this frame write to the texture we just created
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap, 0);

	// we don't want to draw/read any color data, just depth values
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "OmniShadowMap::init() framebuffer status bad " 
			<< status << std::endl;
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void OmniShadowMap::bindToFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject);

	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "OmniShadowMap::bindToFrameBuffer() error "
			<< error << " can't bind to framebuffer: " 
			<< m_frameBufferObject << std::endl;
	}

	glViewport(0, 0, m_width, m_height);

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "OmniShadowMap::bindToFrameBuffer() error " 
			<< error << " can't resize viewport: " << std::endl;
	}

	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << framebufferStatus << std::endl;
	}
}

void OmniShadowMap::read(GLenum textureUnit) {
	glActiveTexture(textureUnit);

	// Directional shadows use an orthogonal transformation, so
	// we use GL_TEXTURE_2D
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "OmniShadowMap::read() error: " 
			<< error << " at m_shadowMap id: " << m_shadowMap 
			<< " textureUnit: " << textureUnit << std::endl;
	}
}
