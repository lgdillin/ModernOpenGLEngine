#pragma once

#include <iostream>

#include <GL/glew.h>

class GBuffer {
public:
	GBuffer();
	~GBuffer();

	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TEXCOORD,
		GBUFFER_NUM_TEXTURES
	};

	bool init(unsigned int _windowWidth, unsigned int _windowHeight);
	bool init2(unsigned int _windowWidth, unsigned int _windowHeight);

	void bindForWriting();

	void bindForReading();

	void setReadBuffer(GBUFFER_TEXTURE_TYPE _textureType);

	GLuint getGBufferObject() { return m_gBufferObject; }
	GLuint getGPosition() { return m_gPosition; }
	GLuint getGNormal() { return m_gNormal; }
	GLuint getGAlbedoSpecular() { return m_gAlbedoSpecular; }
	

private:
	GLuint m_frameBufferObject;
	GLuint m_textures[GBUFFER_NUM_TEXTURES];
	GLuint m_depthTexture;

	GLuint m_gBufferObject;
	GLuint m_gPosition;
	GLuint m_gNormal;
	GLuint m_gAlbedoSpecular; // READ: gColor + Specular
	GLuint m_renderDepthBuffer;
};