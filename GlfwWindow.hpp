#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GlfwWindow {
public:
	GLFWwindow* window;
	GLuint VAO, VBO, shader;
	std::string vShader, fShader;

	void createTriangle() {

		// Vertex Shader
		// Note: location=0 is what we will be referencing
		// with attrib pointers below
		vShader = std::string("#version 330\n")
			+ "layout(location=0) in vec3 pos;\n"
			+ "void main() { gl_Position = "
			+ "vec4(.4*pos.x, .4*pos.y, .4*pos.z, 1.0); }";

		// Fragment shader
		fShader = std::string("#version 330\n")
			+ "out vec4 color;\n"
			+ "void main() { color = vec4(1.0, 0.0, 0.0, 1.0); }";

		GLfloat vertices[] = {
			-1.0f, -1.0, 0.0f, 
			1.0f, -1.0f, 0.0,
			0.0f, 1.0f, 0.0f
		};

		// The amount of arrays we want to create
		// where we actualy want to store the generated ID's 
		// for the arrays
		// Notice how the VAO is just an unsigned integer,
		// And what we are doing is passing its address to the 
		// video card so the video card can modify the data at that address
		// (ie. assign the generated ID for the vertex array memory)
		glGenVertexArrays(1, &VAO);
		// Now we actually signal to the video card what block of memory
		// we want to work with, by passing it the ID for the memory block
		// That was generated earlier (we can work with lots of different blocks)
		glBindVertexArray(VAO);

		// This is the spot where a lot of programmers like to indent
		// to say 'we are now working with this VAO' and then when they are
		// finished they unindent when they unbind the VAO
		
		// Now we need to create a buffer object to actually go inside that
		glGenBuffers(1, &VBO);
		// the VBO has multiple targets it can bind to
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Connect the buffer data (The verts we have just created
		// to the actual GL_ARRAY_BUFFER) for this VBO

		// 1. pick the target we are binding to
		// 2. the size of the data we are passing in
		//	(We can pass in sizeof(), or we can pass in the variable
		//	times the number of elements in the array, can actually be 
		//	better if the array is part of a larger Object)
		// 3. pass the array of vertices itself
		// 4. Static draw - we aren't going to be changing the values
		//	once we put them in there (we can move the triangle around the screen,
		//	but we won't change the values in the array itself)
		//	dynamic draw - allows for meticulous data modification
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// this makes more sense after you write the actual shader
		// 1. the location of the attribute we are creating this pointer for
		//	(recall: layout value we set to 0 in the shader program)
		// 2. size: size of each value thats going to get passed in
		//	we are going to have the (x,y,z) values so the value should be 3
		// 3. The datatype of the value being passed in
		// 4. if we want to normalize the value (true/false)
		// 5. you can splice data together, ie. we can colors and vertices in the same
		//	value (like {x1,y1,z1,r1,g1,b1,x2,y2,z2,r2,g2,b2}). if we were drawing just
		//	the vertices, we might want to skip the rgb values in between and come back later
		//	so in that case we would put a stride of 3, to 'stride' over those values as we read
		// 6. offset - where the data starts, or where we want to pick back up
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		// we want to enable that location=0 we made in shader program
		glEnableVertexAttribArray(0);

		// Unbinding the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Unbind the VAO
		glBindVertexArray(0);

	}

	void compileShaders() {
		// creat the shader program itself
		// The programs themselves are stored on the GPU
		// We are messing with the id
		shader = glCreateProgram();

		// Make sure the shader was created correctly
		if (!shader) {
			std::cout << "Error creating shader" << std::endl;
			return;
		}

		// add the shaders we made to the program
		addShader(shader, vShader, GL_VERTEX_SHADER);
		addShader(shader, fShader, GL_FRAGMENT_SHADER);

		// Get the error codes from the creation of the shaders
		GLint result = 0;
		GLchar errorLog[1024] = { 0 };

		// pass in our id generated from glCreateProgram() to the program linker
		// actually creates the executables on the graphics card
		glLinkProgram(shader);

		// This is actually for debugging
		// A lot of messages and codes are generated
		// 1. pass shader id,
		// 2. which bit of information we want to get back from it (GL_LINK_STATUS)
		// 3. where we want it to store the result of that
		glGetProgramiv(shader, GL_LINK_STATUS, &result);

		if (!result) {
			glGetProgramInfoLog(shader, sizeof(errorLog), NULL, errorLog);
			std::cout << "Error Linking shader program " << errorLog << std::endl;
			return;
		}

		// Checking for us if the shader we created is valid
		glValidateProgram(shader);
		glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(shader, sizeof(errorLog), NULL, errorLog);
			std::cout << "Error validating shader program " << errorLog << std::endl;
			return;
		}


	}

	void addShader(GLuint programId, std::string shaderCode, GLenum shaderType) {
		// create the individual shader
		// create an empty shader for that type
		GLuint _shader = glCreateShader(shaderType);

		// pointer to the first value in the array
		const GLchar* shaderCodePointer[1];
		shaderCodePointer[0] = (GLchar*)shaderCode.c_str();

		// this is needed for the shader
		GLint shaderCodeStrLength[1];
		shaderCodeStrLength[0] = shaderCode.length();

		// 1. the shader we are going to be modifying the code for
		// 2. we are only passing in one shader code
		// 3. the shader 'code' itself, 
		// 4. the character length of the program
		glShaderSource(_shader, 1, shaderCodePointer, shaderCodeStrLength);
		glCompileShader(_shader);

		// Before we attach it to the program itself, we need to do more error checking
		GLint result = 0;
		GLchar errorLog[1024] = { 0 };
		glGetShaderiv(_shader, GL_COMPILE_STATUS, &result);
		if (!result) {
			glGetShaderInfoLog(shader, sizeof(errorLog), NULL, errorLog);
			std::cout << "Error compiling shader type: " 
				<< shaderType << " with error: " 
				<< errorLog << std::endl;
			return;
		}

		// Now that everything is compiled we can attach to the program
		glAttachShader(programId, _shader);
	}

	int create() {
		// Window dimensions
		const GLint WIDTH = 800, HEIGHT = 600;

		// Initialize GLFW
		if (!glfwInit()) {
			std::cout << "GLFW init failed" << std::endl;
			glfwTerminate();
			return 1;
		}

		// Setup GLFW window properties
		// OpenGL version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

		// Core profile - no backwards compatibility
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Allow forward compatibility
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		// create a window
		window = glfwCreateWindow(
			WIDTH,
			HEIGHT,
			"Test Window",
			NULL,
			NULL
		);

		if (!window) {
			std::cout << "GLFW window creation failed" << std::endl;
			glfwTerminate();
			return 1;
		}

		// Get Buffer size information
		int bufferWidth, bufferHeight;
		
		// Size of the main frame buffer we will be using
		glfwGetFramebufferSize(
			window, &bufferWidth, &bufferHeight);

		// Set the Context for GLEW to use
		// if we have multiple windows, we can switch the context
		// For which window we want to draw in
		glfwMakeContextCurrent(window);

		// Allow modern extension features
		// Enable us to access extensions 
		// for advanced development in the future
		glewExperimental = GL_TRUE;

		if (glewInit() != GLEW_OK) {
			std::cout << "glewInit() failed" << std::endl;
			glfwDestroyWindow(window);
			glfwTerminate();
			return 1;
		}

		// Set up Viewport size (opengl not glfw)
		glViewport(0, 0, bufferWidth, bufferHeight);

		// Create our triangle
		createTriangle();
		compileShaders();

		// Make our main loop (until window close)
		while (!glfwWindowShouldClose(window)) {
			// Get and handle user input events
			glfwPollEvents();

			// Clear Window
			// Used to wipe the frame to a "blank canvas"
			// Otherwise we could end up with a lot of artifacts
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			// Specify we want to clear the color data in the buffer
			// we can specify we want to clear the depth data
			glClear(GL_COLOR_BUFFER_BIT);

			// Use the shader program
			// This will grab the shader with this ID
			// After we have created a shader, compiled it, checked it
			// 
			// In production, we would have a bunch of shaders,
			// Draw what we need to with one shader, swap it out,
			// Draw whatever we need with the next one, etc.
			glUseProgram(shader);

			// Use the shader we have bound
			glBindVertexArray(VAO);

			// 1. GL_LINES: Draw only lines (think wireframe)
			//	GL_TRIANGLES: Draw lines and fill each set of 3 coords
			// 2. Where we start indexing at (offset)
			// 3. the amount of points we want to draw
			//	normally we would store how many points there are
			//	in the object we want to draw and feed that arg in
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);

			glUseProgram(0); // Unbinds the shader

			// OpenGL has two buffers/frames
			// One the user sees, and the one that is being drawn
			// We see the next frame by switching the address the pointer
			// Is pointing to
			glfwSwapBuffers(window);

			
		}

		return 0;
	}


private:
};