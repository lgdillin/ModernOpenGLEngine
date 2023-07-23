#version 330

in vec3 texCoords;

out vec4 color;

uniform samplerCube skybox;

void main() {
	// grab the point of the skybox at the location defined by texCoords
	color = texture(skybox, texCoords);
}