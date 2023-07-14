#version 330

layout (location = 0) in vec3 pos;

// how we convert our position vec to the POV of our directional Light
// it will be a combination of the view of the camera, 
// and the perspective (orthogonal)
// i.e projection * view
uniform mat4 directionalLightTransform;
uniform mat4 model;

void main() {
	// think: projection * view * model * vec4(pos, 1.0);
	gl_Position = directionalLightTransform * model * vec4(pos, 1.0);
}