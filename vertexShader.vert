#version 330

layout(location=0) in vec3 pos;
layout(location=1) in vec3 newColor;
layout(location=2) in vec2 textureCoordinatesIn;

out vec4 vertexColor;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 scaleMatrix;


void main() {
	gl_Position = projection * view * model * vec4(pos, 1.0);
	//vertexColor = vec4(clamp(newColor, 0.0, 1.0), 1.0);
	vertexColor = vec4(newColor, 1.0);

	vec4 temp = scaleMatrix * vec4(textureCoordinatesIn, 0.0, 1.0);
	//vec4 temp = vec4(textureCoordinatesIn, 0.0, 1.0);

	texCoords = vec2(temp.x, temp.y);
}