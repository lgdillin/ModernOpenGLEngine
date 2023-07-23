

= Skybox
	- A cube mesh creating the illusion of a larger world
	- use cubemaps to texture the cube
	- requires its own shader to run before the main shader
	- world is drawn 'on top of' the skybox
	- could create a huge skybox around the whole scene
	- but then we'd have to use the same view matrix far plane everywhere
	- solution: disable depthmask

= Skybox - Cubemap
	- Creating cubemap works much like before
	- Bind 6 textures to each face, use RGB or RGBA values instead
	
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
		width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	- Use a separate shader for drawing skybox and pass in a 1x1x1 cube
	- Essentially we are creating a small box around the camera

	- Skybox is a small cube around the camera
	- how can we see objects in the scene? disable the depthmask for the 
		skybox render
	- OpenGL treats skybox depth as being at the furthest possible distance
	- so it renders as if behind the objects

= Skybox - Shader
	- Pass gl_Position with projection and view matrix, but Withouth model matrix:
		we want the skybox to remain static around the camera.

	texCoords = aPos;

	- Texture Coordinates can use fragment position. Since the skybox does not move,
		it is effectively at the origin. So any fragment position vector 
		is also its direction from the origin
	- Fragment Shader takes skybox as a samplerCube
	
	color = texture(skybox, texCoords);

	- Remember: samplerCubes use a vector to find a texel, so we use texCoords'
		vector from Vertex Shader

= Skybox - Drawing
	- Drawing the skybox works like any other draw
		1. Initialize Shader to Use
		2. Bind VAO of skybox cube
		3. bind texture of skybox
		4. draw skybox
	- However, another important step is needed;

	// Call before drawing to disable depth mask
	glDepthMask(GL_FALSE);

	// Do stuff.

	// Important: remember to re-enable the depth mask after!
	glDepthMask(GL_TRUE);

= Skybox - Finalizing
	- When drawing the skybox, need to treat camera as if it is origin (where
		the skybox is treated as being)
	- Therefore, view matrix passed to skybox shader needs to remove any 
		translations it has (rotations are to remain so we can look around 
		our skybox)
	- Recall: translation happens in the 4th column of a matrix
	- So we can just remove the column by converting a mat3, then convert back to 
		a mat4

	glm::mat4(glm::mat3(viewMatrix));

	- Another important note: if you call glClear() before rendering the scene
		itself, make sur eyou do it before rendering the skybox
	- glClear() clears the current FrameBuffer. if you clear after the skybox, you 
		will clear the skybox

= Summary
	- Skybox uses 6 textures around the camera to create illusion of a larger world
	- a skybox is usually just a small cube around the camera
	- Disabling the depthmask before drawing a skybox enables the scene to be drawn
		as if in front of the skybox
	- Need to remove translation of view matrix for skybox call to keep
		camera in skybox
	- make sure to call glClear before the skybox not after it