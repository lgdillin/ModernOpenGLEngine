#include "Texture.hpp"

Texture::Texture() {
	textureId = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";

}

Texture::Texture(std::string _fileLocation) {
	textureId = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = _fileLocation;
}

Texture::~Texture() {
	clearTexture();
}

bool Texture::loadTextureAlphaOption(bool alphaMode) {

	int stbiMode = alphaMode ? STBI_rgb_alpha : STBI_rgb;
	unsigned char* textureData = stbi_load(fileLocation.c_str(), &width,
		&height, &bitDepth, stbiMode);

	if (!textureData) {
		std::cout << "failed to load/find " << fileLocation 
			<< " check location or mode " << std::endl;
		return false;
	}

	// Generate a texture id and bind to the buffer
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	// Handle texture wrapping on the S and T axis (read: X and Y axis)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// Manage the scaling of the texture's look when very close/far
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLint glMode = alphaMode ? GL_RGBA : GL_RGB;

	glTexImage2D(GL_TEXTURE_2D, 0, glMode, width, height, 0,
		glMode, GL_UNSIGNED_BYTE, textureData);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(textureData);

	return true;
}

void Texture::useTextureDebug(GLuint id) {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureId);
}


void Texture::useTexture() {
	// GL_TEXTURE0 is what is called a texture unit
	// When our texture is being run in the frag shader, there will be
	// a sampler. the sampler is what will have access to the data of our 
	// texture, but will be accessing it through our unit.
	// There is a required minimum number of 16 texture units for a 
	// modern GPU, but many GPUs have more
	glActiveTexture(GL_TEXTURE1);
	// binds textureId to GL_TEXTURE0
	glBindTexture(GL_TEXTURE_2D, textureId);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "error binding texture " << error << std::endl;
	}
}

void Texture::clearTexture() {
	glDeleteTextures(1, &textureId);
	textureId = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}