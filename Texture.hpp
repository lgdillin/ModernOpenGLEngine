#pragma once

#include <iostream>
#include <GL/glew.h>
#include "stb_image.cpp"

class Texture {
public:
	Texture();
	Texture(std::string _fileLocation);
	~Texture();

	bool loadTextureAlphaOption(bool alphaMode);
	void useTexture(GLenum _textureUnit = GL_TEXTURE1);
	void useTextureDebug(GLuint id);
	void clearTexture();

	GLuint getTextureId() { return textureId; }

protected:
	int width, height, bitDepth;
	
	GLuint textureId;
	std::string fileLocation;
};