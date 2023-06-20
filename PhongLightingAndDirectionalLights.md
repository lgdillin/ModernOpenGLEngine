
== Phong lighting
	- Consists of 3 parts:
		- Ambient Lighting: Light that is always present, even if a 
			light source's direct path is blocked
		- Diffuse Lighting: Light determined by direction of light
			source. Creates a faded effect further from the light
		- Specular Lighting: Light reflected perfectly from the source
			to the viewer's eye. Effectively a reflection of the light
			source. More prominent on shiny objects
	- Combined, the 3 parts create the "Phone lighting model"
		(Phong reflection is another name for phong lighting)

= Ambient Lighting
	- Simplest lighting concept
	- simulates light bouncing off other objects
	- example: create a shadow on the ground with your hand, using the 
		sun.. you can still see the color in the shadow (it is still
		being lit)
	- Global illumination simulates this

	- Create an ambient lighting factor
	
	ambient = lightColor * ambientStrength;

	- This factor is how much of a fragment's color this lights ambient shows

	fragColor = objectColor * ambient;

	- if ambient is 1.0 (full power) then the fragment is always fully lit
	- if ambient is 0 (no power) then the fragment is always black
	- if ambient is 0.5 (half power) then the fragment is half its normal color

= Diffuse Lighting
	- More complex
	- simulates the drop-off of lighting as angle of lighting becomes
		more shallow
	- side facing directly at a light is brightly lit
	- side facing at an angle is more dim
	- can use the angle between the vector connecting light source
		to fragment and the vector perpendicular to the face 
		(surface 'normal')
	- Uses the value of theta between the light source and the normal
		of the surface to determine the brightness (smaller theta => more light)

	- Recal from vectors: dot product

	dot(v1,v2) = length(v1) * length(v2) * cos(theta)

	- if both vectors are normalized (converted to unit vectors)
		then length(v1) = length(v2) = 1.
		therefore, v1*v2 = cos(theta)
	- Since cos(0 deg) = 1 and cos(90 deg) = 0,
		we can use the output of dot(v1, v2) to determine a diffuse factor

	- if factor is negative, then the light is behind the surface, so 
		default to 0
	- Apply diffuse factor with ambient:

	fragColor = objectColor * (ambient + diffuse);
= Diffuse Lighting - Normals
	- Normals are vectors perpindicular to their point on a surface
	- Can define them for each face
	- Each vertex will have multiple normals, one for each face which
		it is a part of
	- Good for 'flat shading', not good for realistic smooth shading
	- also doesn't work too well for indexed draws: we only define
		each vertex once per face

	- Alternative: Phong SHADING (not phong lighting)
	- Phong lighting is a name of the entire model, phong shading is
		an interpolation method
	- Instead of having multiple normals for each vertex, we instead
		have an average of all normals which it is a part of
	- Each vertex has an average of the normals of all the surfaces
		it is a part of
	- Interpolate between these averages in shader to create smooth effect
	- Good for complex models
	- find the normals of all the faces a vertex is attached to, add them
		all together, then divide them by the number of faces
	- Not so good for simple models with sharp edges (unless you use some
		clever modeling techniques)

	- Phong shaded sphere is defined the same way as flat shaded
	- Smoothness is illusion created by interpolating and effectively
		'faking' surface normals to look curved

	- Problem with non-uniform scales
	- Wrongly skewing normals
	- Can be countered by creating a 'normal matrix' from model matrix

	- Transform normals with:

	glm::mat3(transpose(inverse(model)));

	full explanation:
	http://lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/

= Specular lighting
	- Specular relies on the position of the viewer
	- it is the direct reflection of the light source hitting the viewer's eye
	- moving around will affect the apparent position of the specular reflection
		on the surface
	- Therefore, we need four things
		- Light vector
		- Normal vector
		- reflection vector (light vector reflected around normal)
		- View vector (vector from viewer to fragment)
	- Need the angle between the viewer and the reflection
		(smaller angle -> more light)

	- View vector is just the difference between the Fragment Position
		and the Viewer (Camera) position
	- Reflection vector can be obtained with a built-in GLSL function

	// incident: vector to reflect
	// normal: normal vector to reflect around
	reflect(incident, normal);

	- Just as with diffuse, use dot product between normalized forms of
		view and reflection vector, to get specular factor

= Specular Lighting - Shininess
	- One last step to alter specular factor: shininess
	- Shininess creates a more accurate reflection
	- Higher shine: smaller more compact specular
	- Lower shine: larger, faded specular
	- Simply put, previously calculated specular factor to the power
		of shininess values

	specularFactor = (dot(view, reflection)).power(shininess);

	- Then apply the Specular Factor just as with ambient and diffuse

	fragColor = objectColor * (ambient + diffuse + specular);

= Types of Light
	- Directional Light: A light without a position or source. All light
		is coming as parallel rays from a seemingly infinite distance.
		Best example is the Sun.
	- Point Light: A light with a position that shines in all directions
		Best example is a lightbulb
	- Spot Light: Similar to a Point Light, but cut down to emit in a 
		certain range, with a certain angle. Best example is a flashlight
	- Area Light: More advanced light type that emits light from an area.
		Think of a large light up panel on a wall or ceiling

= Directional Light
	- Simplest form of light
	- Only requires basic information (color, ambient, diffuse, specular)
		and a direction
	- Treat all light calculations using the same direction for the
		light vector
	- Don't need to calculate a light vector!

= Summary
	- Phong Lighting model combines ambient, diffuse and specular lighting
	- diffuse and specular require normals
	- Use dot product and normals to determine diffuse lighting
	- Use dot product and light reflected around normaals to determine
		specular lighting
	- Phon shading interpolates avergaed normals to create smooth surfaces
	- four main types of light: directional, point, spot, area
	- directional light is easiest since it only requires a direction and
		allows us to skip calculating a light vector