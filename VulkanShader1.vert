#version 450

//vec2 positions[3] = vec2[](
//	vec2(0.0, -0.05),
//	vec2(0.05, 0.05),
//	vec2(-0.05, 0.05)
//);
//
//vec3 colors[3] = vec3[](
//	vec3(1.0, 0.0, 0.0),
//	vec3(0.0, 1.0, 0.0),
//	vec3(0.0, 0.0, 1.0)
//);

layout (location = 0) in vec2 in_vertexPosition;
layout (location = 1) in vec3 in_vertexColor;

layout (push_constant) uniform constants {
	mat4 model;
} u_objectData;

layout(location = 0) out vec3 v_out_fragColor;

void main() {
	//gl_Position = objectData.model * vec4(positions[gl_VertexIndex], 0.0, 1.0);
	//v_out_fragColor = colors[gl_VertexIndex];

	gl_Position = u_objectData.model * vec4(in_vertexPosition, 0.0, 1.0);
	v_out_fragColor = in_vertexColor;
}