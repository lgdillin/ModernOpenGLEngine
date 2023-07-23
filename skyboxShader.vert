#version 330

layout (location = 0) in vec3 pos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {

	// Interpolated texture coordinate at that point
	texCoords = pos;

	// Interpolated value in the fragment shader
	gl_Position = projection * view * vec4(pos, 1.0);
}