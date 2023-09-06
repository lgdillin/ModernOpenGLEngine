#version 330 core

layout (location = 0) in vec3 layout_position;
layout (location = 1) in vec3 layout_color;
layout (location = 2) in vec2 layout_texCoords;
layout (location = 3) in vec3 layout_normals;

uniform mat4 u_projection;
uniform mat4 u_model;
uniform mat4 u_view;

void main() {
	gl_Position = u_projection * u_view * u_model * vec4(layout_position, 1.0);
}