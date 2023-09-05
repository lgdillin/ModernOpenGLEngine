/*
	Using Glew (Graphics Library Extension Wrangler):
	#include <GL/glew.h>

	after initialising the OpenGL Context:
	glewExperimental = GL_TRUE;
	glewInit();
	Should return GLEW_OK. if it fails, returns an error
	You can read the error with glewGetErrorString(result);

	GLEW can check if extensions exist:
	if(!GLEW_EXT_framebuffer_object) {}

	wglew.h for Windows-specific functions
*/

/*
	GLFW: OpenGL FrameWork 
	- Handles window creation and control
	- Pick up and process input from the keyboard, mouse, joystick, and gamepad
	- Even allows multiple monitor support
	- Uses OpenGL context for windows

*/

/*
	SDL: Simple DirectMedia Layer
	- Can do almost everything GLFW can do
	- Also (Audio, Threading, Filesystems, etc)
	- Very popular, especially for indie devs
	- Used in: FTL, Amnesia, Starbound, and Dying Light
	- Even used in level editors for Source Engine and Cryengine
*/

/*
	SFML: Simple and Fast Multimedia Library
	- Like SDL but with even more features
	- The OpenGL context is very weak. Based on 2D only graphics

	GLUT: OpenGL Utility Toolkit
	- No longer maintained. Avoid using

	Win32 API
	- Lowest level for window creation (for MS Windows)
	- Only attempt if you know what you're doing
*/

/*
RENDERING THEORY/PIPELINE

	What is the rendering pipeline?
	- A series of stages that take place in order to render an image
	on the screen

	- 4 stages are programmable via "Shaders". 
		Vertex Shader
		Fragment Shader
		Geometry Shader
		Tesselation Shader
		(new 5th stage) Compute Shader (appeared in 2012 OpenGL 4.3)

	- Shaders are pieces of code written in OpenGL Shading Language 
	(GLSL), or High-Level Shading Language (HLSL)- if Direct3D is being used

	- GLSL is based on C

	- The full Rendering Pipeline is split up into 9 stages
		1. Vertex Specification
		2. Vertex Shader (programmable) (Must be defined)
		3. Tessellation (programmable)
		4. Geometry Shader (Programmable)
		5. Vertex Post-Processing
		6. Primitive Assembly
		7. Rasterization
		8. Fragment Shader (programmable)
		9. Per-Sample Operations
	
	- If a stage is not programmed explicitly, the data is passed through

	== Vertex Specification
	- A vertex is a point in space, usually defined as (x,y,z)
	- A primitive is a simple shape defined using one or more vertices
	- Usually we use triangles, but we can also use points, lines and quads
	- Vertex Specification: Setting up the data of the vertices for 
	the primitives we want to render (Done in the application itself)
	- Uses Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
	- VAO defines WHAT the data a vertex can 
	have: (position, color, texture, normals, etc.)
	- VBO is the data itself
	- Attribute Pointers define where and how shaders can access 
	the vertex data.

	- Vertex Specification: Creating a VAO/VBO
		1. Generate a VAO ID
			- when you create a VAO on a graphics card, even though
			it dedicates the memory for the VAO to hold, you can't directly
			access that data.
			- What is actually returned is an ID, which can be used to query 
			the graphics card with the ID, and tell it what you want it to do
		2. Bind the VAO with that ID
			- Before you use the VAO, after you generate the id, you then 
			have to bind the VAO with that ID
		3. Generate a VBO Id
			- Same story with the VBO as the VAO
		4. Bind the VBO with that ID 
			- Now you're working on the chosen VBO attached to the chosen VAO
			- Because you bound the VAO first, OpenGL will assume that the VBO
			you want to work with will be the VBO in the previously bound VAO
			(Im assuming that when a VAO is generated, it also generates the necessary
			memory automatically so we can bind the VBO)
			- (Some programmers will bind the VAO, then indent the code, and then
			assume everything is being worked with that VAO, then unindent after unbinding)
		5. Attach the vertex data to that VBO
		6. Define the Attribute Pointer formatting
			- so how the actual data in that VBO is formatted
			(groups of 3, collation, etc. ), is it floats/integers,
			all is defined there
		7. Enable the Attribute Pointer so it can be accessed
		8. Unbind the VAO and VBO, ready for the next object to be bound
		- For example, we've bound the data needed to draw a cube, and its done,
		so now we need to bind again in order to draw a pyramid

	- Vertex Specification: Initiating a Draw
		1. Activate Shader Program you want to use
			- bunch of predefined code (VS code, FS code) 
			so the pipeline knows what we are about to draw
		2. Bind VAO of object you want to draw
		3. call glDrawArrays(...), which initiates the rest of the pipeline

	== Vertex Shader
		- Handles vertices individually
		- NOT optional
		- Must store/define something in gl_Position as it is used by later stages
		- Can specify additional outputs that can be picked up and used by 
		user-defined shaders later in pipeline
		- Inputs consist of the vertex data itself

	- Vertex Shader: Simple Example
		// Define version of GLSL you're using 
		// syncd with OpenGL 3.30 in our application 
		// Latest version is OpenGL 4.6, so we can say '#version 460'
		// If we allow forward compatibility in our application
		#version 330  

		// by explicity defining the location of these 'in' variables
		// We dont have to query their location back in the application code
		layout(location=0) in vec3 pos;
		void main() {
			gl_Position = vec4(pos, 1.0);
		}

	== Tessellation (Introduced in OpenGL 4.0)
		- Allows you to divide up data into smaller primitives
		- Relatively new shader type, appeared in OpenGL 4.0
		- Can be used to add higher levels of detail dynamically
		- (not a beginner technique)

	== Geometry Shader
		- Where the Vertex Shader handles individual vertices one
		at a time, Geometry Shader handles primitives 
		(groups of vertices)
		- Takes primitives (ie. a triangle) then "emits" (read: outputs)
		their vertices (as a primitive (ie. triangle)) to create the 
		given primitive, or even new primitives (ie. duplicate
		- Can alter the primitive type (points, lines, triangles, etc.)

	== Vertex Post-Processing
		- Transform Feedback (if enabled), Result of Vertex and 
		Geometry stages saved to buffers for later use
		- Clipping: Primitives that won't be visible are removed
		- Positions converted from "clip-space" to "window-space"

	== Primitive Assembly
		- Vertices are converted into a series of primitives
		- So if rendering triangles: 6 vertices would become 2 triangles
		- Face culling: the removal of primitives that can't be seen,
		or are 'facing away' from the viewer.

	== Rasterization
		- Converts primitives to "Fragments". Not exactly a "pixel", but rather
		the data for each pixel, obtained from the rasterization process
		- Fragments are pieces of data for each pixel, obtained from the 
		rasterization process
		- Sort of where the vertex for that particular fragment would be if it
		were a vertex still. 
		- Fragment data will be interpolated based on its position relative
		to each vertex. 

	== Fragment Shader
		- Where most of the calculations take place
		- Fragment shader will go through all of the fragments we have, so each individual pixel,
		and perform an operation on that. The fragment shader is where we can choose
		to output the actual color of that pixel
		- The FS is where we will be handling all the lighting, texturing, and shadowing
		- Handles data for each fragment
		- Is optional but it's rare to not use it. Exceptions are cases
		where only depth or stencil data is required (more on depth data later)
		- Most important output is the color of the pixel that the fragment covers
		- Simplest OpenGL programs usually have a Vertex Shader and Fragment Shader

	- Fragment Shader: Simple Example
		#version 330
		// Fragment shader only really ever has one output
		// Output is always assumed to be the color, regardless of name
		out vec4 color;

		// If anything was passed out from the Vertex Shader,
		// We would be able to capture it as an input to this shader
		void main() {
			color = vec4(1.0, 0.0, 0.0, 1.0);
		}

	== Per-Sample Operations
		- Series of tests run to see if the fragment should be drawn
		- Most important test: Depth test. Determines if something is in front
		of the point being drawn
		- Color Blending: Using defined operations, fragment colors are "blended"
		together with overlapping fragments. Usually used to handle transparent objects
		- Fragment data written to currently bound Framebuffer (usually the default 
		buffer, more on this later)
		- Lastly, in the application code the user usually defines a buffer swap here,
		putting the newly updated frameBuffer to the front
		- The pipeline is complete

	== On the origins of Shaders
		- Shader programs are a group of shaders (vertex, tessellation,
		Geometry, Fragment.. ) associated with one another
		- They are created in OpenGL via a series of functions

	== Creating a Shader Program
		1. Create empty program
		2. Create empty shaders (VS, FS, etc.)
		3. Attach shader source code to shaders
		4. Compile data for those shaders
		5. Attach compiled shaders to program
		6. Link Program (creates executables from shaders and links
		them together)
		7. Validate Program (checking that everything is working)
		(optional but highly advised because debugging shaders in a 
		pain) - can't be breakpointed because it runs on GPU

	== Using a Shader Program
		- When you create a shader, an ID is generated (like with VAOs/VBOs)
		- Simply call glUseProgram(m_shaderId)
		- All draw calls from then on will use that shader, glUseProgram
		is used on a new m_shaderId, or on '0' (meaning 'no shader') as a safety

	== Summary:
		- Rendering Pipeline consists of several stages
		- Four stages are programmable via shaders (Vertex, Tessellation,
		Geometry, Fragment)
		- Vertex shader is mandatory
		- Vertices: User-defined points in space
		- Primitives: Groups of vertices that make a simple shape
		(usually a triangle)
		- Fragments: Per-pixel data created from primitives
		- Vertex Array Object (VAO): What data a vertex has
		- Vertex Buffer Object (VBO): The vertex data itself
		- Shader programs are created with at least a Vertex Shader and then 
		activated before use
*/

/*
VECTORS, MATRICES, AND UNIFORM VARIABLES
	
*/
//#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <SDL.h>

// Developed libraries
//#include "SdlWindow.hpp"
#include "GlfwWindow.hpp"
#include "GlfwGame.hpp"
#include "GlfwWindow1.hpp"

int main(int argc, char* argv[]) {
	/*SdlWindow sdlWindow;
	sdlWindow.create();*/

	//GlfwWindow glfwWindow;
	//glfwWindow.createWindow();

	GlfwGame glfwGame;
	glfwGame.init(); 
	glfwGame.runGame();


	//GlfwWindow1 glfw;
	//glfw.create();

	
	return 0;
}

/*
= Objectives Learned
	- how to create and manipulate windows
	- how to draw meshes/models
	- how to use shaders
	- how to use uniform variables
	- how to use GLM to transform models
	- how to use indexed draws
	- how to use projections
	- how to create and control a camera
	- how to map textures 
	- how to use the phong lighting model
	- how to implement directional, point, and spot lights
	- how to import models
	- how to create and use shadow maps
	- how to implement a skybox

*/