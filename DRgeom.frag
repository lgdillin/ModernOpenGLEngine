#version 330

// gBuffer data prefixed with 'g'
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpecular;

in vec2 v_out_texCoords;
in vec3 v_out_normals;
in vec3 v_out_fragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
	// store the fragment position vector in the first gBuffer texture 
	gPosition = v_out_fragPos;

	// Also store the per-fragment normals into the gBuffer 
	gNormal = normalize(v_out_normals);

	// as well as the per-fragment diffuse color 
	gAlbedoSpecular.rgb = texture(texture_diffuse1, v_out_texCoords).rgb;

	// store specular intensity in gAlbedoSpecular's alpha component 
	gAlbedoSpecular.a = texture(texture_specular1, v_out_texCoords).r;
}