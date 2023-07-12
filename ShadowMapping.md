

== Shadow mapping
	- Literally create a 'map' of the shadows made by a light
	- use this mpa to determine where not to apply light
	- the map is held as a 2D texture (sampler2d in shader)
	- Map is created using a 'framebuffer'
	- framebuffer then writes to a texture
	- Therefore: at least two rendering passes are needed
	- one for creating a shadow map, second for drawing scene

= Shadow mapping - creating map
	- First pass: render the scene from perspective of a light source
	
	- shaders don't just create color output
	- recall from rendering pipeline: PER-SAMPLE OPERATIONS
	- Depth tests using depth buffer values
	- depth buffer is another buffer along with color buffer that
		holds a value between 0 and 1 that determines how deep
		into a frustum a fragment is
		(frustum is the box that defines what the camera can see)
	- 0 is on the Near Plane (close to the camera)
	- 1 is on the far plane (far from the camera)

	- Extract the depth buffer data using the FRAMEBUFFER objecvt
	- normally, framebuffer is bound to '0'
	- this is the default buffer (the one drawn to the screen 
		when buffer swap is called)
	- We can find a separate framebuffer and draw to that, then use the
		data as we wish

	// create a frame buffer
	glGenFramebuffer(1, &frameBufferObject);

	// create a texture the usual way, except..
	// GL_DEPTH_COMPONENT is a single float value as apposed to
	// RGB which is 3 values.
	// Data is NULL, so we have created an empty texture with 
	// dimensions width * height.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, 
		height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	- How to use the framebuffer
	- set framebuffer to write to texture with:

	// GL_DEPTH_ATTACHMENT tells framebuffer to only write depth
	// buffer data.
	glFrameBufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, textureId, 0);

	// These override color data draw/read operations. we don't want to
	// output color with our shadow map
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);


	- Shader itself is simple:
		1. Apply projection and View matrices as if light source is camera
		2. apply model matrix of each object
		3. Fragment shader isn't even needed: depth buffer is written
			automatically.

	- Directional light shadow map works differently to point/spot 
		shadow maps
	- view matrix position should consist of reverse of diretional light's
		direction (simulating light in that direction)
	- View matrix direction is simple the direction of the light
	- (we don't want to use a perspective projection, we want to
		use orthographic projection)
	- Projection matrix is different: Frustum of perspective projection
		fans out. Directional light rays are all parallel, the must not
		fan out.
	- Solution: Orthographic Projection matrix

	glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.01f, 100.0f);

= Shadow Mapping - using map
	- After rendering the scene with the shadow map shader,
		the texture bound to it is occupied with the shadow map data.
	- Make sure to unbind the framebuffer used for the shadow map
	- now we need to bind the texture to our main shader and use it

	- Need access to the View Matrix used in the shadow map shader
		(the one using the light's perspective)
	- use this to get the curretn fragment position in relation to
		the light source
	- Need to create a way to access points on the shadow map
		with the light source perspective's fragment coordinates
	- Therefore, need to convert light source perspective fragment's 
		coords to 'normalized device coords' (values between -1
		and 1)

	- Need to perform a 'perspective divide'
	- similar to how coords are created when moving to the fragment 
		shader anyway
	- However this is only applied to gl_Position.
	- we need to do it manually for the position relative to the
		light source
	- Easy calculation: divide vector by it's 'w' component. 
		this is why vec4 is important
	
	
	vec3 projectedCoords = LightSpacePos.xyz / LightSpacePos.w;

	- Then we need to scale the projectedCoords to 0,1 to match the 0,1
		values of a texture (recall textures use u- and v- axis 
		between 0 and 1)
	
	projectedCoords = (projectedCoords * 0.5) + 0.5;

	- Now use texture function to get closest depth measured during 
		shadow map pass
	
	float closest = texture(shadowMap, projecteCoords.xy).r

	- Grab z values from projectedCoords
	- z-axis on normalizes coords is between [0,1] just like depth,
		and so can be treated as such
	- Compare current and closest depth
	- If current larger than closest: it is further away than the first
		point the light hits at that fragment.  So it must be in 
		the shadow.
	- Otherwise: it is the same point, so it must be getting lit
		by the light
	- To apply shadow, simply add or remove diffuse and specular 
		(retain ambient, remember: Ambient Light is ALWAYS present)

	color = fragmentColor * (ambient + (1.0 - shadow) 
    	* (diffuse + specular);

= Shadow Mapping - Shadow acne
	- Shadow acne occurs due to resolution issues
	- imagine lighting a surface at an angle
	- When rendering from a less slanted angle, two pixels may converge
		to one texel on the shadow map
	- one point could be mistaken as being behind a point next to it

	- Solution: add a slight bias
	- Effectively moving everything slightly towards the camera to
		fake a closer depth
	- Try to keep the bias small or 'peter panning' occurs
		(objects appear floating above their shadow)
	- Bias offset causes areas close to shadow source to disappear because
		depth values are close

= Shadow Mapping - Oversampling
	- What about areas outside of the projection frustum used to create
		the shadow map?
	- Values will be outside [0,1] range and therefore always create shadows
	- Solution:
		- Set texture type to use border with values all consisting of 0
			(always lowest depth value so always lit)
		- For values beyond far plane and therefore greater than 1:
			initialize to 0.

= Shadow Mapping - PCF
	- Edges of shadows are limited to resolution of the  texture 
		which the shadow map is written to
	- can causes pixelated edges
	- Solution: sample surroundding texels and calculate average.  
		apply only partial shadows for shadowed areas
	- Also known as: Percentage-closer filtering (PCF)
	- can become very expensive

	- Get depth values of surrounding texels, such as the 8 immediate
		surrounding
	- Determine if in shadow
	- if yes: increment shadow value
	- when done, divide shadow value by number of samples taken
	- Apply percentage of shadow using this value

	(eg. shadow value calculated as 3, and 9 samples are taken.
		3/9 = 0.33 so apply 33% shadow to that pixel)

	- More samples: better fade effect
	- Keep in mind, this set of samples will be taken for every fragment,
		so instead of being one calculation, it becomes 9x calculations
		just for using immediate surrounding texels

= Summary 
	- Shadows created by texture maps of depth data
	- Depth data created by rendering scene from point of view of light source
	- do 2 passes: one to craete shadow maps and one to render scene
	- compare depth of fragment from light's perspective to value on 
		shadow map texture
	- add bias to remove shadow acne
	- set values from beyond sampling region to '0' (no shadow)
	- use PCF algorithms to fade shadow edges