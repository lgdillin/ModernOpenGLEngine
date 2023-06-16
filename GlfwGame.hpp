#pragma once

#include <iostream>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GlfwWindow.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"

class GlfwGame {
private:
	std::vector<Mesh*> meshes;
	std::vector<Shader*> shaders;

	std::string vShader, fShader;

	GLuint uniformModel, uniformProjection;

	GlfwWindow* glfwWindow;

public:
	GlfwGame() {
		meshes = std::vector<Mesh*>();
		shaders = std::vector<Shader*>();

		std::string vShader = "vertexShader.vert";
		std::string fShader = "fragmentShader.frag";

		uniformModel = 0;
		uniformProjection = 0;

		glfwWindow = NULL;
	}

	// Have some stuff to get the loop started
	void init() {
		// Initialize GLFW
		if (!glfwInit()) {
			glfwTerminate();
			throw("glfwInit() failed");
		}
		glewExperimental = GL_TRUE;

		glfwWindow = new GlfwWindow(800, 600);

		GLfloat vertices[] = {
			-1.0f, -1.0, 0.0f, // location
			1.0f, 0.0f, 0.0f, // color 

			1.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

			0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f
		};

		// create a set of indices to tell the GPU which points
		// to place in which order
		unsigned int indices[] = {
			0, 3, 1,
			1, 3, 2,
			2, 3, 0,
			0, 1, 2
		};

		// Initialize meshes
		Mesh* mesh = new Mesh();
		mesh->createMesh(vertices, indices, 24, 12);
		meshes.push_back(mesh);

		Mesh* mesh1 = new Mesh();
		mesh1->createMesh(vertices, indices, 24, 12);
		meshes.push_back(mesh1);

		createShaders();

		// Initialize glfw
		//if (!glfwInit()) {
		//	glfwTerminate();
		//	throw("glfwInit() failed");
		//}

		// initialize glfw-based window
	}

	void createShaders() {
		Shader* shader1 = new Shader();
		shader1->createFromFile(vShader, fShader);
		shaders.push_back(shader1);
	}

	void runGame() {
		bool directionToggle = true;

		const float toRadians = 3.14159265f / 180.0f;
		float triangleOffset = 0.0f;
		float triangleMaximumOffset = 0.5f;
		float triangleIncrementalValue = 0.0002f;

		float incrementArgument = 1.0f;
		float sinArgument = 1.0f;
		float sinColor = 0.0f;

		while (!glfwWindow->getShouldClose()) {
			// Get and handle user input events
			glfwPollEvents();

			if (directionToggle) {
				triangleOffset += triangleIncrementalValue;
			} else {
				triangleOffset -= triangleIncrementalValue;
			}

			if (abs(triangleOffset) >= triangleMaximumOffset) {
				directionToggle = !directionToggle;
			}

			// Clear Window
			// Used to wipe the frame to a "blank canvas"
			// Otherwise we could end up with a lot of artifacts
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			// Specify we want to clear the color data in the buffer
			// we can specify we want to clear the depth data
			// Use the bitwise OR '|' to clear both color buffer bit
			// and the depth buffer bit
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Use the shader program
			// This will grab the shader with this ID
			// After we have created a shader, compiled it, checked it
			// 
			// In production, we would have a bunch of shaders,
			// Draw what we need to with one shader, swap it out,
			// Draw whatever we need with the next one, etc.
			//glUseProgram(shader);
			shaders[0]->useShader();
			uniformModel = shaders[0]->getModelLocation();
			uniformProjection = shaders[0]->getProjectionLocation();

			sinArgument += 0.03f;
			incrementArgument += 0.05f;
			sinColor += .25f;

			glm::mat4 model(1);


			model = glm::translate(model,
				glm::vec3(triangleOffset, 0.8f, -3.0f));
			model = glm::rotate(model, sinArgument * toRadians,
				glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, 
				glm::value_ptr(glfwWindow->getProjection()));
			meshes[0]->renderMesh();

			model = glm::mat4(1);
			model = glm::translate(model,
				glm::vec3(-triangleOffset, -0.8f, -3.0f));
			model = glm::rotate(model, (sinArgument)*toRadians * 1.8f,
				glm::vec3(0.5f, 0.6f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
			meshes[1]->renderMesh();

			glUseProgram(0); // Unbinds the shader

			// OpenGL has two buffers/frames
			// One the user sees, and the one that is being drawn
			// We see the next frame by switching the address the pointer
			// Is pointing to
			glfwWindow->swapBuffers();


		}
	}
};