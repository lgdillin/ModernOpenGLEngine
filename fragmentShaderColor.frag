#version 330

in vec4 vertexColor;
in vec2 texCoords;

out vec4 color;

struct DirectionalLight {
	vec3 color;
	float ambientIntensity;
};

uniform sampler2D texture0;
uniform DirectionalLight directionalLight;

void main() {
	vec4 ambientColor = vec4(directionalLight.color, 1.0) 
		* directionalLight.ambientIntensity;


	// Color only mode
	color = vertexColor; 

	// Texture-only mode
	//color = texture(texture0, texCoords);

	// Texture and Color mod
	//color = texture(texture0, texCoords) * ambientColor;
	//color = texture(texture0, texCoords) * vertexColor;
}