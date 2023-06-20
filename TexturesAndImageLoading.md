

== Textures
	- Textures are images used to add extra detail to an object
	- Textures can also be used to hold generic data
	- Usually 2d but can also have 1D and 3D textures (volumetric mapping)
	- Points on textures are 'texels' not pixels
	- texels are defined between 0 and 1

= Textures (Texels)
	- So to sample a point at the top middle of a texture, you reference
		texel (0.5, 1)
	- Map texels to vertices
	- Interpolation over each fragment will calculate appropriate texels in 
		between the assigned texels

= Texture Objects
	- Creating textures works much like creating VBOs/VAOs
	- glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
	- There are different types of texture, such as:
		GL_TEXTURE_1D
		GL_TEXTURE_2D
		GL_TEXTURE_3D
		GL_TEXTURE_CUBE_MAP
		(and more)

	- Once you have actually bound it, you can actually write to it
	- glTexImage2D(GL_TEXTURE_2D, GL_RGB, width, height, 
		0, GL_RGB, GL_UNSIGNED_BYTE, data);

	- 1: Texture target (what target we have bound our texture to and will operate on)
	- 2: Mipmap level.
		Mipmap level 0: Uses the same texture across the whole range
		Mipmap level 1:

	- 3: Format of the stored data. RGB, RGBA, and more
	- 4: Width
	- 5: Height
	- 6: (Should always be 0) Legacy concept handling texture border no 
		longer used by OpenGL
	- 7: Format of the data being loaded (as opposed to stored on 
		the third argument)
	- 8: The data type of the values (int, float, byte, etc.)
	- 9: the data itself

= Mipmaps
	- Resolution limitations for textures
	- The closed we get to an object, the more pixelated the texture becomes. 
		Further away, it attempts to render multiple texels on one pixel
	- Solution: create multiple versions of the image at different resolutions
		and switch between them based on distance

= Texture Parameters - Filters
	- What if we try to render off center of texels?
	- Two possibilities
		- Nearest: use the texel with the most overlap (creates a pixelated effect)
		- Linear: Use a weighted average of surrounding texels (blends pixel boundaries)
			(samples from nearby texels and creates a 'blurred' average of texels)
	- Linear is more common in most applications
	- Nearest used if you want a pixelated effect (such as retro games) 

	- glTexParameter: used to set texture rendering parameters
	- glTexParameteri: version of the function used for setting an integer value
	- glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	- glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	- GL_TEXTURE_MIN_FILTER: Filter to apply when the texture is made smaller
		(ie. further away)
	- GL_TEXTURE_MAG_FILTER: Filter to apply when the texture is made bigger
		(ie. is closer)

	- GL_LINEAR: Linear filter (blends surrounding texels)
	- GL_NEAREST: Nearest filter (picks nearest texel to sampling point)

	- Using GL_NEAREST, has a more pixelated look, good for certain visual styles
	- Using GL_LINEAR, blends texels together to create a smoother look. works well
		on complex textures, less so on simple ones

= Texture Parameters - Wrap
	- What if we try to sample a point outside the [0,1] range?
	- Multiple ways to handle it:
		- Repeat the texture
		- Repeat a mirror form of the texture (flipping back and forth)
		- Extend the pixels at the edge
		- apply a contoured border
	- Can use glTexParameter to define how this is handled

	glTextParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GL_TEXTURE_WRAP_S: How to handle wrapping on the 's-axis' (x-axis)
	GL_TEXTURE_WRAP_T: How to handle wrapping on the 't-axis' (y-axis)

	GL_REPEAT: Repeat texture
	GL_MIRRORED_REPEAT: Repeat and mirror texture
	GL_CLAMP_TO_EDGE: Extends pixels at edge
	GL_CLAMP_TO_BORDER: Apply colored border

= Load Images for Textures
	- You could build your own image loader
	- But will get confusing for handling eahc image type (bmp, jpg, etc..)
	- Image Loader Libraries do the work for us
	- Popular library: Simple OpenGL Image Library (SOIL)
	- For the sake of simplicity, we can use a smaller library:
	- stb_image

= Using stb_image
	- only requires the header file
	- Must start project with:

	#define STB_IMAGE_IMPLEMENTATION

	unsigned char* data = 
		stbi_load("image.jpg", &width, &height, &bitDepth, 0);

	- Might need to flip image
	stbi_set_flip_vertically_on_load(true);

= Texture Samplers
	- Samplers are an ID which points to a texture unit, and the texture unit
		has the texture applied to it.
	- Textures in Shaders are accessed via "Samplers"
	- Textures are attached to a 'texture unit'
	- Samplers access textures attached to their texture unit
	- in shader, use 'sampler2D' type
	- To get the value of a texel, use GLSL 'texture' function
	
	texture(textureSampler, TexCoord);

	- textureSampler: the sampler2D object
	- TexCoord: the interpolated texel co-ordinate in the fragment shader

= Texture Unit
	- Example:
		- Texture 'brick.png' is attached to Texture Unit 0
		- sampler2D: 0 reads from texture attached to Texture Unit 0 (by binding ID)

	- Bind texture to desired Texture Unit:
		
		glActiveTexture(GL_TEXTURE0); // GL_TEXTURE0 is the texture unit
		// Any texture bound while GL_TEXTURE0 is active, will get bound to
		// GL_TEXTURE0 texture unit
		glBindTexture(GL_TEXTURE_2D, textureId);

	- Ensure sampler2D variables know which texture unit to access:
	
		// uniformTextureSampler will now access whatever texture is bound to
		// texture unit GL_TEXTURE0
		glUniform1i(uniformTextureSampler, 0);

		// The value attached to the uniform is the Texture Unit Number

= Summary
	- Textures use texels between 0 and 1
	- Texels are bound to vertices and values are interpolated
	- Mipmaps handle level of detail more efficiently
	- Texture filtering chagnes how texels are blended (or aren't blended)
		based on size on screen
	- Texture wrapping changes how txtures are handled for texel values outside
		of the 0,1 range
	- Wrapping and Filtering are defined using the glTexParameteri function
	- Load images with 3rd party libraries for convenience
	- SOIL is a popular library for this but stb_image is more lightweight
	- Textures attach to texture units, samplers read from textures attached
		to texture units