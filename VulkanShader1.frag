#version 450

layout(location = 0) in vec3 v_out_fragColor;

layout(location = 0) out vec4 f_out_color;

void main() {
	f_out_color = vec4(v_out_fragColor, 1.0);
}