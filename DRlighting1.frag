#version 330

in vec2 v_out_texCoords;

out vec4 f_out_fragColor;

uniform sampler2D u_gPosition;
uniform sampler2D u_gNormal;
uniform sampler2D u_gAlbedoSpecular;

struct Light {
	vec3 position;
	vec3 color;
	float linear;
	float quadratic;
};

const int NUMBER_LIGHTS = 1;
uniform Light u_lights[NUMBER_LIGHTS];
uniform vec3 u_viewPos;

void main() {
	//f_out_fragColor = texture(u_gNormal, v_out_texCoords);
	//f_out_fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	f_out_fragColor = texture(u_gAlbedoSpecular, v_out_texCoords);
}