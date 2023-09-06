#version 330

layout (location = 0) out vec4 f_out_fragColor;

uniform vec3 u_lightColor;

void main() {
	f_out_fragColor = vec4(u_lightColor, 1.0);
}