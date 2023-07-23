#version 330

in vec4 fragPosition;

uniform vec3 lightPosition;
uniform float farPlane;

void main() {
	// assumes nearPlane is roughly 0
	float normalizedDistance = length(fragPosition.xyz - lightPosition) / farPlane;

	gl_FragDepth = normalizedDistance;
}