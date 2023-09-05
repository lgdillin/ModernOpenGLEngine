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

	// retrieve data from gBuffer 
	vec3 gFragPosition = texture(u_gPosition, v_out_texCoords).rgb;
	vec3 gNormal = texture(u_gNormal, v_out_texCoords).rgb;
	vec3 gDiffuse = texture(u_gAlbedoSpecular, v_out_texCoords).rgb;
	float gSpecular = texture(u_gAlbedoSpecular, v_out_texCoords).a;

	//gDiffuse = (gDiffuse * 0.5) + 0.5;

	// then calculate lighting as usual 
	vec3 lighting = gDiffuse * 0.1; // hard-coded ambient component.
	vec3 viewDir = normalize(u_viewPos - gFragPosition);

	for(int i = 0; i < NUMBER_LIGHTS; ++i) {

		// diffuse 
		vec3 lightDir = normalize(u_lights[i].position - gFragPosition);
		vec3 diffuse = max(dot(gNormal, lightDir), 0.0) 
			* gDiffuse * u_lights[i].color;

		// Specular 
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float specMultiplier = pow(max(dot(gNormal, halfwayDir), 0.0), 64.0);
		vec3 specular = specMultiplier * u_lights[i].color * gSpecular;

		// attenuation 
		float dist = length(u_lights[i].position - gFragPosition);
		float attenuation = 1.0 / (
			1.0 // constant factor
			+ u_lights[i].linear * dist // linear factor
			+ u_lights[i].quadratic * dist * dist // quadratic factor
		);

		diffuse *= attenuation * 1.2;
		specular *= attenuation;
		lighting += diffuse + specular;
	}

	//f_out_fragColor = texture(u_gAlbedoSpecular, v_out_texCoords) * vec4(lighting, 0.0);
	f_out_fragColor = vec4(lighting, 1.0);

	//gl_FragDepth = length(gFragPosition.xyz - u_lights[0].position) / 500.0;
	//f_out_fragColor = vec4(0.0, 0.0, 1.0, 1.0);
}