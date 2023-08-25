#include "GBuffer.hpp"

GBuffer::GBuffer() {
	m_frameBufferObject = 0;
	//m_textures = {};
	m_depthTexture = 0;

	m_gBufferObject = 0;
	m_gPosition = 0;
	m_gNormal = 0;
	m_gAlbedoSpecular = 0;
	m_renderDepthBuffer = 0;
}

GBuffer::~GBuffer() {
}

bool GBuffer::init(unsigned int _windowWidth, unsigned int _windowHeight) {
	// Create FBO
	glGenFramebuffers(1, &m_frameBufferObject);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBufferObject);

	// create the gbuffer textures
	//GLsizei numTextures = sizeof(m_textures) * GBUFFER_NUM_TEXTURES;

	// the number of elements in the array m_textures is currently set
	// to GBUFFER_NUM_TEXTURES
	GLsizei numTextures = GBUFFER_NUM_TEXTURES;
	glGenTextures(numTextures, m_textures);
	glGenTextures(1, &m_depthTexture);


	for (int i = 0; i < numTextures; ++i) {
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _windowWidth, _windowHeight,
			0, GL_RGB, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
			GL_TEXTURE_2D, m_textures[i], 0);
	}

	// depth
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, _windowWidth,
		_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		m_depthTexture, 0);

	GLenum drawBuffers[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};

	glDrawBuffers(sizeof(drawBuffers), drawBuffers);


	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GBuffer::init() failed with error: " << status << std::endl;
		return false;
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	return true;

}

bool GBuffer::init2(unsigned int _windowWidth, unsigned int _windowHeight) {
	// configure gBuffer framebuffer
	glGenFramebuffers(1, &m_gBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferObject);

	// Position buffer
	glGenTextures(1, &m_gPosition);
	glBindTexture(GL_TEXTURE_2D, m_gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _windowWidth, _windowHeight,
		0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, m_gPosition, 0);

	// normal buffer
	glGenTextures(1, &m_gNormal);
	glBindTexture(GL_TEXTURE_2D, m_gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _windowWidth, _windowHeight,
		0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, m_gNormal, 0);

	// albedo (color) + specular buffer
	// specular value is stored in the alpha channel within the target
	// GL_RGBA
	glGenTextures(1, &m_gAlbedoSpecular);
	glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _windowWidth, _windowHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
		GL_TEXTURE_2D, m_gAlbedoSpecular, 0);

	// Tell OpenGL which color attachment we'll use (of this
	// framebuffer) for rendering
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &m_renderDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_renderDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
		_windowWidth, _windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, m_renderDepthBuffer);

	// check for completeness/errors
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GBuffer::init2() failed with error: " << status << std::endl;
		return false;
	}
}

void GBuffer::bindForWriting() {
}

void GBuffer::bindForReading() {
}

void GBuffer::setReadBuffer(GBUFFER_TEXTURE_TYPE _textureType) {
	glReadBuffer(GL_COLOR_ATTACHMENT0 + _textureType);
}
