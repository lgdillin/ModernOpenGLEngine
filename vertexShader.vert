#version 330

layout(location=0) in vec3 pos;
layout(location=1) in vec3 newColor;

out vec4 vertexColor;

uniform mat4 model;
uniform mat4 projection;

void main() {
	gl_Position = projection * model * vec4(pos, 1.0f);
	vertexColor = vec4(clamp(newColor, 0.0f, 1.0f), 1.0f);
}