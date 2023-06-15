

== Interpolation
	+ idea of interpolation is that as youre calling the vertex shader,
		you only work with the individual vertices, but what about the vertices
		between those?  how does the frag shader know how to handle those?
		(hint: interpolation)

	- Per-Vertex attributes passed on are 'interpolated' using the other values
		on the primitive
	- in other words: A weighted average of the three vertices on a triangle
		is passed on
	- Fragment Shader picks up the interpolated value and uses that
	- The value is effectively an estimate of what the value should be at that
		position, had we defined it ourself
	- Interpolation is something that happens during the RASTERIZATION stage

	- Classic example, using position coords as RGB values
	- Top of triangle is green, because in (x,y,z) y is hight
	- convert to RGB then green is high
	- midway between red and green areas, colors blend, but we didn't
		define that vertex position (the value was interpolated)

	- Interpolation is used for quickly and accurately estimating values without
		defining them
	- Can be used for interpolating texture coordinate when mapping textures
	- can be used for interpolating normal vectors when handling lighting
	- especially useful in Phong shading to create the illusion of 
		smooth/rounded surfaces

== Indexed draws
	+ imagine we want to draw a cube. 

	- Imagine we want to draw a cube.  Define vertices to draw a cube
	- Cube will consist of 12 triangles (two for each face)
	- 12 * 3 vertices per triangle = 36 vertices
	- but a cube only has 8 vertices
	- some will be defined multiple times (very messy)
	- Instead, why not define the 8 vertices of the cube 
	- number them 0 to 7, and refer to them by their number

	- to accomplish this, create an Element Array Buffer in the VAO
	- bind them to the EAB in the VAO
	- glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		(Element buffer object -> EBO, Index buffer object -> IBO)
	- Sometimes they're called an 'element' instead of an 'index'
	- The next level of this is 3d Models

== Projections
	- How we see things/how we visualized
	- Used to convert from 'view space' to 'clip space'
	- Can be used to give the scene '3D' look
	- Alternatively can be used to create 2d style for projects that require it
	- need to understand coordinate systems

	- To Create a Clip Space we define an area (FRUSTUM) of what is not
		to be clipped with a projection matrix
	- 2 commonly used types of projection:
		- Orthographic (most common in 2d applications) 
			(Things are drawn in straight lines)
		- Perspective (most common in 3d applications)
			(lines are drawn in a sort of 'fan' effect)

= Projections: Coordinate Systems
	- Local Space: Raw position of each vertex drawn relative to origin.
		Multiply by Model matrix to get

	- World Space: Position of vertex in the world itself if camera is assumed
		to be positioned at the origin. Multiply by View Matrix to get

	- View Space: Position of vertex in the world, relative to the camera position
		and orientation. Multiply by Projection Matrix to get

	- Clip Space: Position of vertex in the world, relative to the camera position
		and orientation, as viewed in the area not to be 'clipped' from the final output

	- Screen Space: After clipping takes place, the final image is created and placed
		 on the coordinate system of the window itself

= Projections: Orthographic
	- The frustrum for the orthographic projection is cuboid
	- Everything between the Far Plane and Near Plane is kept. Rest is discarded
	- Parallel nature of orthographic means 3D depth is not visible
	- Move object closer/further and it won't change size on screen

= Projections: Perspective
	- The frustrum for perspective projections is a 'truncated pyramid'
	- Each pixel on Near Plane diverges at an angle to reach matching point 
		on the Far Plane
	- Gives the illusion of depth

= Projections Comparison
	- Orthographic: The one furthest back looks to be the same size as the one
		in the front, implying its actually larger
	- Perspective: The one at the back looks smaller than the one at the front
		due it being more distant, as it should

= Projections with GLM and OpenGL
	- glm::mat4 proj = glm::perspective(fov, aspect, near, far);

	- fov := field of view, the angle of the frustum
	- aspect := aspect ratio of the viewport (usually its width / height)
	- near := distance of the near plane
	- far := distance of the far plane
	- Bind the given matrix to a uniform in the shader

	- gl_Position = projection * view * model * vec4(pos, 1.0);

== Summary
	- Interpolation calculates weighted values between vertices during rasterization
	- Indexed Draws let us define vertices once then reference them to draw them
	- Projection Matrices convert View Space to Clip Space
	- Orthographic Projections are used for 2D application and don't allow depth perception
	- Perspective Projections are for 3D applications and create the illusion of depth
	- GLM has glm::perspective function to create perspective matrices