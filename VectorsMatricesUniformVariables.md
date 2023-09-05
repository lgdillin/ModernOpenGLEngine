
Vectors, Matrices, and Uniform Variables in OpenGL

== Vector Overview
	- A quantity with magnitude and direction
	- Can be used for lots of things, normally to represent a direction,
	or something's position (eg. how far and in what direction 
	something is, relative to a certain point)
	
	Cool vector identity:
	dot(v1, v2) = magnitude(v1) * magnitude(v1) * cos(theta)
	magnitude(v1) = sqrt(v1.x**2 + v1.y**2 + v1.z**2)

	dot(v1, v2) / (mag(v1) * mag(v2)) = cos(theta)
	cos^-1( dot(v1, v2) / (mag(v1) * mag(v2)) = theta

	- Scalar projection: dot product with assumption that b is a unit vector
	mag(a) * cos(theta) := a projection of a onto b's axis
	mag(a) * cos(90) = 0, thus a and b would be orthogonal

	- unit vector: u has same direction as v but with a mag() of one
	u = v / mag(v)

	- cross product

== Matrix Overview
	- Matrix multiplication is actually filling a new matrix with a series of 
	dot products

	- What a translation looks like:
	[1 0 0 X] [x] [x + X]
	[0 1 0 Y] [y] [y + Y]
	[0 0 1 Z]X[z]=[z + Z]
	[0 0 0 1] [1] [  1  ]

	- Matrix scaling
	[SX 0  0  0] [x] [SX*x]
	[0  SY 0  0] [y] [SY*y]
	[0  0  SZ 0]X[z]=[SZ*z]
	[0  0  0  1] [1] [  1 ]

	- Matrix Rotation
		- Rotation rotates a vector
		- should be thought of as rotating around its origin
		- So to choose a point of rotation, translate the vector so
			the point to rotate around is at the origin
		- three different matrices for handling rotation

		(Note: angle must be in radians, not degrees)
		- X Rotation
		[1 0    0     0] [x] [       x       ]
		[0 cosΘ -sinΘ 0] [y] [cosΘ*y - sinΘ*z]
		[0 sinΘ cosΘ  0]X[z]=[sinΘ*y + cosΘz ]
		[0 0    0     1] [1] [       1       ]

		- Y rotation
		[cosΘ  0 sinΘ 0] [x] [cosΘ*x + sinΘz  ]
		[0     1 0    0] [y] [        y       ]
		[-sinΘ 0 cosΘ 0]X[z]=[-sinΘ*x + cosΘ*z]
		[0     0 0    1] [1] [        1       ]

		- Z rotation
		[cosΘ -sinΘ 0 0] [x] [cosΘ*x - sinΘ*y]
		[sinΘ cosΘ  0 0] [y] [sinΘ*x + cosΘ*y]
		[0    0     1 0]X[z]=[       z       ]
		[0    0     0 1] [1] [       1       ]

	- Matrix Transforms: combining transforms can be as easy as multiplying 
		them together, then applying it to the vector
	- Order matters! 
	- Transforms happen in reverse order: The scale is applied first, then the translation
	- if you swapped them around, the translation would be applied, and then the scale,
		which would result in scaling the translation

== GLM

	- GLM is a free library for handling common mathematical operations used with opengl
	- most importantly: vectors and matrices
	- Uses vec4 (vector with 4 values) and mat4 (4x4 matrix)
	- simple code:
		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 3.0f));

== Uniform Variables
	- Type of variable in shader
	- 'in' variables are passed as a per-vertex basis
	- uniform variables stay the same for every single vertex which
		gets passed through the shader
	- uniform value may be useful for passing the same value to every vertex
	- Uniforms are values global to the shader that aren't associated with a
		particular vertex

	- Example:
		#version 330
		in vec3 pos;
		uniform mat4 model; // Could be a translation matrix
		void main() {
			gl_Position = model * vec4(pos, 1.0);
		}

	- Each uniform has a location ID in the shader
	- Need to find the location so we can bind a value to it
	- Example:
		int location = glGetUniformLocation(m_shaderId, "uniformVarName");

	- Now we can bind a value to that location:
		glUniform1f(location, 3.5f);

	- Make sure you have set the appropriate shader program to use!

	- Different variable types for binding values, for example:
		glUniform1f - single floating value
		glUniform1i - single integer value
		glUniform4f - vec4 of floating values
		glUniform4fv - vec4 of floating values, value specified by pointer
		glUniformMatrix4fv - mat4 of floating values, value specified by pointer
		etc..

== Summary
	- Vectors are direction and positions in space
	- matrices are 2d arrays of data used for calculating transforms and 
		various other functions
	- vectors are a type of matrix and can have these functions applied to them
	- the order of transform operations matters
	- Last matrix operation applied happens first
	- GLM is used to handle matrix calculations
	- uniform variables pass global data to shaders
	- need to obtain a uniforms location then bind data to it