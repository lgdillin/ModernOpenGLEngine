#version 330

layout (location = 0) in vec3 layout_position;
layout (location = 1) in vec3 layout_color;
layout (location = 2) in vec2 layout_texCoords;
layout (location = 3) in vec3 layout_normals;

out vec2 v_out_texCoords;

void main() {
	v_out_texCoords = layout_texCoords;
	gl_Position = vec4(layout_position, 1.0);
}