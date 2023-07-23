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
	void useTexture();
	void useTextureDebug(GLuint id);
	void clearTexture();


protected:
	int width, height, bitDepth;
	
	GLuint textureId;
	std::string fileLocation;
};