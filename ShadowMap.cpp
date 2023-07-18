#include "ShadowMap.hpp"

ShadowMap::ShadowMap() {
	m_frameBufferObject = 0;
	m_shadowMap = 0;

	m_width = 0;
	m_height = 0;
}

ShadowMap::~ShadowMap() {
	if (m_frameBufferObject) {
		glDeleteFramebuffers(1, &m_frameBufferObject);
	}

	if (m_shadowMap) {
		glDeleteTextures(1, &m_shadowMap);
	}
}

bool ShadowMap::init(unsigned int _width, unsigned int _height) {
	m_width = _width;
	m_height = _height;

	glGenFramebuffers(1, &m_frameBufferObject);
	
	glGenTextures(1, &m_shadowMap);
	glBindTexture(GL_TEXTURE_2D, m_shadowMap);

	// GL_DEPTH_COMPONENT
	// so normally, we use GL_RGB (3 values). GL_DEPTH_COMPONENT tells us
	// how deep into the image it is (a value between 0,1) based on the far plane
	// say far plane = 100. if pixel is found at 50, then the Depth component
	// will be set at 0.5.  
	// Right now we are not passing a data array, we want an initialized buffer
	// because we have no image (will be created with FBO)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_width, m_height,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	
	// Manage the scaling of the texture's look when very close/far
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Handle texture wrapping on the S and T axis (read: X and Y axis)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//float bColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor);


	// Bind as a GL_FRAMEBUFFER
	// THere are also GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBufferObject);

	// So this is the thing that will connect our framebuffer to our texture
	// so when our FB is updated, when it gets rendered to, it will get rendered
	// to the texture we pass to it
	// GL_DEPTH_ATTACHMENT: use the depth values when its writing to our texture
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, m_shadowMap, 0);

	glDrawBuffer(GL_NONE); // we don't draw on the first pass
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer error: " << status << std::endl;
	}

	//glBindTexture(GL_TEXTURE_2D, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void ShadowMap::bindToFrameBuffer() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBufferObject);
	glViewport(0, 0, m_width, m_height);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "bind to frame buffer " << error << std::endl;
	}
}

void ShadowMap::read(GLenum textureUnit) {
	glActiveTexture(textureUnit);

	// Directional shadows use an orthogonal transformation, so
	// we use GL_TEXTURE_2D
	glBindTexture(GL_TEXTURE_2D, m_shadowMap);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "bind texture to shadowmap " << error << std::endl;
	}
}
