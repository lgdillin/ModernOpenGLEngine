

= Omnidirectional Shadow Maps
	- Used for Point Lights and Spot Lights
	- Basic theory same as regular Shadow Maps
	- But need to handle shadows in EVERY direction
	- Can't just use a single texture
	- Need MULTIPLE textures to handle every direction
	- Solution: A Cubemap

= Cubemaps
	- Type of texture in OpenGL
	- Technically exists as 6 textures (one for each face) but can be referenced
		in GLSL as if only a single texture
	
	// instead of binding to GL_TEXTURE_2D, we bind to GL_TEXTURE_CUBE_MAP
	// when we want to apply data to it, we have to say which side we want to apply
	// the data to
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ...); // we can iterate in a loop

	// The enums are ordered as shown below
	GLTEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GLTEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GLTEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,

	- Each enum is one increment of the previous, so can loop through with 
		incrementing value
	- when we create a shadowmap, we have to bind the texture as null, just as before

	- for cube maps, we don't need the (u,v) values
	- can access point on cube map with direction vector pointing to texel on cube map,
		from center of cube
	- This means we don't need a light transform matrix for each point light
	- however we need 6 version of the 'projection * view' matrix, one for each of the
		6 directions on the cube, for shadow map pass
	- our projection will always be the same but our view will change for each face

= Omnidirectional Shadow Maps
	- Using Perspective Projection instead of orthographic
	
	glm::perspective(glm::radians(90.0f), aspect, near, far);

	- 90 degree perspective ensures all 360 degrees around one axis can be covered
	- Aspect is the width of one side of the cube, divided by its height.
		This SHOULD be '1' for it to work properly. (all cube dimensions are equal)
	- Near and far decide size of the cube (how far the light reaches)

	- to create our shadow maps, we need 6 light transformations with projection
		matrix, one for each direction

	projection * upViewMatrix,
	projection * downViewMatrix,
	...

	- Create view matrices using light position and direction.

	glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));

	- Direction is 'lightPos + glm::vec3(1.0, 0.0, 0.0)' becuase it points to the 'right',
		in other words: positive x direction

	- IMPORTANT: These matrices must line up with the Cubemap texture order:
		(POSITIVE_X, NEGATIVE_X, POSITIVE_Y, ...)

= Geometry Shader
	- Vertex Shader only needs to do World Space transformation (ie. multiply 
		vertex by model matrix)
	- Projection and View will be applied in the geometry shader
	- Geometry Shader is another shader type that happens between vertex and
		fragment shader
	- Geometry shader handles primitives (points, lines, triangles, etc.)

	- Geometry shaders can also create entirely new primitives
	- Vertex shaders handle individual vertices
	- Geometry shader handles groups of vertices and can manipulate entire primitives
	- Can also create entirely NEW primitives
	- Don't explicitly specify output variables
	- Instead use functions like 'EmitVertex()' and 'EndPrimitive()'

	// Creates a vertex at location stored in gl_Position
	EmitVertex();

	// Stores primitives created by last EmitVertex() calls,
	// and starts a new primitive
	EndPrimitive();

	- Example Geometry shader


	#version 330
	layout (triangles) in;
	layout (triangle_strip, max_vertices=3) out;

	void main() {
		for(int i = 0; i < gl_in.length(); ++i) {
			gl_Position = gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}


	- 'layout (triangles) in' specifies incoming primitive type is a triangle
	- Output is essentially the same but also specifies the number of vertices 
		expected with it. if you ouput more than the max, they won't be rendered
	- gl_in stores data for each vertex passed from Vertex Shader

	- One other value: gl_Layer
	- Since we attach a cubemap to the framebuffer, the framebuffer has multiple 
		layers: one for each output texture in the cubemap
	- set the values of gl_Layer to determine which one to write to
		when calling EmitVertex().

	- this is useful, because it means we can move everything into is respective place
		based on the view
	- Using the 6 transformation matrices and reassigning gl_Layer for each face
	- We can render each object 6 times for each of the directions of the light source
		in one render pass
	- Alternative: Do 6 shader render passes and swithc out the light transformation
		matrix each time (bad practice)

= Using the Shadow Cubemap
	- GLSL has type 'samplerCube' (bind cubemap to this)
	- When using texture, instead of uv coords, supply direction vector
	- Use direction of light source to fragment being checked, no need for a light
		transformation matrix
	- Using far plane, convert depth value to actual value

	float closest = texture(depthmap, fragToLight).r;
	closest *= far_plane;

	- Then compare this value to the length of fragToLight (distance from fragment
		to light source), and use that to determine if in shadow

= Multiple Point Lights - Common Mistakes
	- In theory its easy: one samplerCube for each point light
	- Shadow map pass is done as previously stated
	- render pass is also done as previously stated, however
	- by default, sampelrs are mapped to GL_TEXTURE0
	- if you already have a sampler2D mapped to GL_TEXTURE0,
		and you have an array of unused point lights,
		Their samplerCubes will remain as default (ie. GL_TEXTURE0)
	- OpenGL forbids different types of sampler to be bound to the same Texture Unit
	- Solution: Find a way to ensure all sampler types have unique Texture Units

= Omnidirectional Shadow Maps - PCF
	- Essentially the same concept, but with a 3rd dimension (vec3)
	- Could just do as before, but with 3rd dimension it becomes intensive
	- and a lot of the samples will be very close to the original
	- One solution: pre defined offset directions that are likely to be well spaced
	
	- Another optimization: The pre-defined offsets are DIRECTIONS, not relative positions
	- we can scale how far we sample in a direction.
	- so we can scale how far a sample is, based on viewer distance
	- if user is close: sample more close to original vector
	- if user is distant: sample more distant from original vector
	- Essentially creating our own filter

= Summary 
	- Omnidirectional Shadows use cubemaps to map shadows in all directions
	- cubemaps are texture consisting of 6 sub-textures
	- cubemap texels are referenced by a direction vector
	- geometry shader handles primitives
	- geometry shader can modify primitives and create new ones. we use it to map
		to the cubemap from 6 views, allowing only a single shadow pass per light
	- Due to nature of cubemaps, no need for light transform matrix in render pass
	- PCF can use predefined offset directions
	- PCF can scale offsets basd on viewer distance
	- Need to ensure samplerCubes aren't bound to same texture unit as sampler2D