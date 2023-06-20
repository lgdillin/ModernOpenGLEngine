== Camera/View Space
	- The camera processes the scene through the "View Space"
	- View Space is the coordinate system with each vertex as seen
		from the camera
	- Use the View Matrix to convert from World Space to View Space
	- View Matrix requires 4 values: Camera Position, Direction, 
		Right, and Up

	- Camera Position: Simple the position of the camera
	- Direction: The direction the camera is looking in
	- Direction vector actually points in opposite direction of
		the "intuitive direction"
	- Right: Vector facing right of the camera, defines x-axis.  Can
		calculate by doing cross product of Direction and 
		Up Vector [0, 1, 0]
	- Up: Upwards relative to where the camera is facing. Can calculate
		by doing cross product of Direction and Right Vectors

= Calculating View Matrix
	- Place values in marix to calculate View Matrix
	- View Matrix applied to a vertex will convert it to view space

	[Rx Ry Rz 0] [1 0 0 -Px]
	[Ux Uy Uz 0] [0 1 0 -Py]
	[Dx Dy Dz 0]X[0 0 1 -Pz]
	[0  0  0  1] [0 0 0  1]

	- Fortunately, GLM has a function to do all of this
	- glm::mat4 viewMatrix = glm::lookAt(position, target, up);

= GLM LookAt()
	- glm::lookAt(position, target, up);
	- position = Camera's position
	- target = Point for camera to look at
	- target is usually defined as the camera's position with a direction added 
		onto it. Effectively saying "look in front"
	- up = The upwards direction of the WORLD, not the camera. lookAt uses this to
		calculate 'right' and 'up' relative to the camera.

= Using the View Matrix
	- Bind the View Matrix to a uniform on the shader
	- Apply it between the projection and model matrices
	- gl_Position = projection * view * model * vec4(pos, 1.0);

	- Remember: order matters
	- Multiplying the projection, view, and model matrices in different order 
		will not work

= Input: Moving the Camera
	- Just need to change camera position!
	- GLFW: glfwGetKey(window, GLFW_KEY_W)

	- SDL: Check for event, check if KEYDOWN event, check which key pressed (check code video)

	- Then add value to camera position while key held

= Input: Delta Time
	- Basic Idea: Check how much time has passed since the last loop, apply maths
		based on this to keep consistent speeds

	- deltaTime = currentTime - lastTime; lastTime = currentTime;
	- Then multiply the camera's movement speed by deltaTime
	- (check https://gafferongames.com/post/fix_you_timestep)

= Input: Turning
	- 3 types of angles
	- Pitch: Looking up and down
	- Yaw: looking left and right
	- Roll: Like an airplane doing a barrel roll
	- Pitching needs to rotate the view up and down using an axis relative to the yaw
	- Yaw will only ever rotate us around our up axis

= Input: Turning - Pitch
	- Pitching axis will depend on yaw - need to update x,y,z
	- y = sin(pitch)
	- x = cos(pitch)
	- z = cos(pitch)

	- Remember we're updating x and z, because the yaw could have the camera
		facing along a combination of them

= Input: Turning - Yaw
	- We could base yaw on pitch too, but would be unrealistic
	- we only update x and z
	- x = cos(yaw)
	- z = sin(yaw)

= Input: Turning - Pitch and Yaw
	- Combine the values from pitch and yaw to get a direction vector 
		with those properties
	- x = cos(pitch) * cos(yaw)
	- y = sin(pitch)
	- z = cos(pitch) * sin(yaw)

	- Vector [x,y,z] will have the given pitch and yaw
	- Update Camera direction with new vector

= Input: Turning
	- GLFW: glfwSetCursorPosCallback(window, callback);
	- Store old mouse position, comapre to new position. Use difference to 
		decide pitch/yaw change

	- SDL: Check for SDL_MOUSEMOTION event.
	- Call SDL_GetMouseState(&x, &y); and then do the same as above

= Summary
	- View Matrix requires Position, Direction, Right, and Up vectors
	- glm::lookAt handles it for us
	- To move camera, alter position on key press
	- DeltaTime allows consistent speed across systems
	- turning uses pitch and yaw (and roll in some cases)
	- Use pitch and Yaw to calculate new direction vectors
	- compare last and ucrrent mouse positions to determine how pitch and yaw change