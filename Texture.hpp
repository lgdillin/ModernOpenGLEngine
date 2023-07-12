#pragma once

#include <iostream>
#include <GL/glew.h>
#include "stb_image.h"

class Texture {
public:
	Texture();
	Texture(std::string _fileLocation);
	~Texture();

	bool loadTextureAlphaOption(bool alphaMode);
	void useTexture();
	void clearTexture();


private:
	int width, height, bitDepth;
	
	GLuint textureId;
	std::string fileLocation;
};