#version 330

layout(location=0) in vec3 pos;
layout(location=1) in vec3 newColor;
layout(location=2) in vec2 textureCoordinatesIn;
layout(location=3) in vec3 normalCoordinatesIn;

// We are doing phong shading and interpolating the normals across
// The surface
// The other technique is flat shading

out vec4 vertexColor;
out vec2 texCoords;
out vec3 normals;
out vec3 fragmentPosition;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 scaleMatrix;


void main() {
	gl_Position = projection * view * model * vec4(pos, 1.0);
	//vertexColor = vec4(clamp(newColor, 0.0, 1.0), 1.0);
	vertexColor = vec4(newColor, 1.0);

	vec4 temp = scaleMatrix * vec4(textureCoordinatesIn, 0.0, 1.0);
	//vec4 temp = vec4(textureCoordinatesIn, 0.0, 1.0);

	texCoords = textureCoordinatesIn;
	
	// Pass the normal
	// normals don't really change during a tranlsation,
	// but they do change during a non-uniform scale, or a rotation
	// So we don't need multiply our normals by the transform of our model,
	// since the transforms are held in the 4th column, we can cast
	// our model into a mat3
	// Also, if you introduce a non-uniform scale, it can affect
	// The normals in such a way that they end up bunched up
	// So we invert and transpose the matrix
	normals = mat3(transpose(inverse(model))) * normalCoordinatesIn;

	fragmentPosition = vec3(model * vec4(pos, 1.0));
}