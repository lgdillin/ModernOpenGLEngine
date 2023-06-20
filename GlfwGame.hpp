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
#include "GlfwCamera.hpp"
#include "Texture.hpp"
#include "RectangularPrism.hpp"
#include "Light.hpp"

class GlfwGame {
private:
	std::vector<Mesh*> meshes;
	std::vector<Shader*> shaders;
	std::vector<RectangularPrism*> prisms;

	std::string vShader, fShader;

	GLuint 
		uniformModel, 
		uniformProjection, 
		uniformView,
		uniformScaleMatrix,
		uniformAmbientIntensity,
		uniformAmbientColor;

	GLfloat deltaTime, lastTime;

	GlfwWindow* glfwWindow;
	GlfwCamera glfwCamera;

	Texture dirtTexture, brickTexture, grassTexture;

	Light simpleLight;

public:
	GlfwGame() {
		meshes = std::vector<Mesh*>();
		shaders = std::vector<Shader*>();
		prisms = std::vector<RectangularPrism*>();

		uniformModel = 0;
		uniformProjection = 0;
		uniformView = 0;

		deltaTime = 0.0f;
		lastTime = 0.0f;

		glfwWindow = NULL;
		
	}

	// Have some stuff to get the loop started
	void init() {
		// Initialize GLFW

		glfwWindow = new GlfwWindow(800, 600);
		glfwWindow->initialize();

		glfwCamera = GlfwCamera();

		GLfloat vertices[] = {
			-1.0f, -1.0, 0.0f, // location
			1.0f, 0.0f, 0.0f, // color 
			0.0f, 0.0f, // texture

			0.0f, -1.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.5f, 0.0f,

			1.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, 0.0f,

			0.0f, 1.0f, 0.0f,
			0.5f, 0.0f, 0.5f,
			0.5f, 1.0f
		};

		// create a set of indices to tell the GPU which points
		// to place in which order
		unsigned int indices[] = {
			0, 3, 1,
			1, 3, 2,
			2, 3, 0,
			0, 1, 2
		};

		simpleLight = Light(1.0f, 1.0f, 1.0f, 0.3f);

		brickTexture = Texture("resources/brick.png");
		brickTexture.loadTexture();
		dirtTexture = Texture("resources/dirt.png");
		dirtTexture.loadTexture();
		grassTexture = Texture("resources/grass.png");
		grassTexture.loadTexture();

		std::string vShader = "./vertexShader.vert";
		std::string fShader = "./fragmentShader.frag";
		std::string fShader1 = "./fragmentShaderColor.frag";

		Shader* shader1 = new Shader();
		shader1->createFromFile(vShader, fShader);
		shaders.push_back(shader1);

		Shader* shader2 = new Shader();
		shader2->createFromFile(vShader, fShader1);
		shaders.push_back(shader2);


		RectangularPrism* rp = new RectangularPrism();
		rp->pickColor("custom", glm::vec3(0.0f, 0.5f, 0.0f));
		rp->setTexture(grassTexture);
		rp->setTranslationVector(glm::vec3(0.0f, 0.0f, 0.0f));
		rp->setRotationAngle(glm::radians(90.0f));
		rp->setRotationVector(glm::vec3(1.0f, 0.0f, 0.0f));
		rp->setScalingVector(glm::vec3(10.0f, 10.0f, 1.0f));
		prisms.push_back(rp);

		// Initialize meshes
		RectangularPrism* rp1 = new RectangularPrism();
		rp1->pickColor("red");
		rp1->setTexture(brickTexture);
		rp1->setTranslationVector(glm::vec3(0.0f, 3.0f, 4.5f));
		rp1->setRotationAngle(0.0f);
		rp1->setRotationVector(glm::vec3(0.0f, 1.0f, 0.0f));
		rp1->setScalingVector(glm::vec3(10.0f, 5.0f, 1.0f));
		prisms.push_back(rp1);

		RectangularPrism* rp2 = new RectangularPrism();
		rp2->pickColor("blue");
		rp2->setTexture(brickTexture);
		rp2->setTranslationVector(glm::vec3(0.0f, 3.0f, -4.5f));
		rp2->setRotationAngle(0.0f);
		rp2->setRotationVector(glm::vec3(0.0f, 1.0f, 0.0f));
		rp2->setScalingVector(glm::vec3(10.0f, 5.0f, 1.0f));
		prisms.push_back(rp2);

		RectangularPrism* rp3 = new RectangularPrism(brickTexture);
		rp3->pickColor("red");
		rp3->setTexture(brickTexture);
		rp3->setTranslationVector(glm::vec3(-4.5f, 3.0f, 0.0));
		rp3->setRotationAngle(glm::radians(90.0f));
		rp3->setRotationVector(glm::vec3(0.0f, 1.0f, 0.0f));
		rp3->setScalingVector(glm::vec3(8.0f, 5.0f, 1.0f));
		prisms.push_back(rp3);

		RectangularPrism* rp4 = new RectangularPrism(brickTexture);
		rp4->pickColor("red");
		rp4->setTexture(brickTexture);
		rp4->setTranslationVector(glm::vec3(4.5f, 3.0f, 0.0f));
		rp4->setRotationAngle(glm::radians(90.0f));
		rp4->setRotationVector(glm::vec3(0.0f, 1.0f, 0.0f));
		rp4->setScalingVector(glm::vec3(8.0f, 5.0f, 1.0f));
		prisms.push_back(rp4);


		// This one is the pyramid
		RectangularPrism* rp5 = new RectangularPrism(dirtTexture);
		rp5->pickColor("white");
		rp5->setTexture(dirtTexture);
		rp5->setTranslationVector(glm::vec3(0.0f, 3.0f, 0.0f));
		rp5->setRotationAngle(0.0f);
		rp5->setRotationVector(glm::vec3(0.0f, 1.0f, 0.0f));
		rp5->setScalingVector(glm::vec3(1.0f, 1.0f, 1.0f));
		prisms.push_back(rp5);

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
			GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter?
			deltaTime = now - lastTime; // 1000*(now - lastTime) / SDL_GetPerformanceFrequency();
			lastTime = now;

			// Get and handle user input events
			glfwPollEvents();
			glfwCamera.keyControl(glfwWindow->getKeys(), deltaTime);
			glfwCamera.mouseControl(glfwWindow->getMouseDeltaX(),
				glfwWindow->getMouseDeltaY());

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
			// glUniform1i(glGetUniformLocation(shaders[0]->getShaderId(),"texture0"), 0);
			uniformModel = shaders[0]->getModelLocation();
			uniformProjection = shaders[0]->getProjectionLocation();
			uniformView = shaders[0]->getViewLocation();
			uniformScaleMatrix = shaders[0]->getScaleMatrixLocation();
			uniformAmbientColor = shaders[0]->getAmbientColorLocation();
			uniformAmbientIntensity = shaders[0]->getAmbientIntensityLocation();

			simpleLight.useLight(uniformAmbientIntensity, uniformAmbientColor);

			sinArgument += 0.03f;
			incrementArgument += 0.05f;
			sinColor += .25f;


			// render/update loop
			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
				glm::value_ptr(glfwWindow->getProjection()));
			glUniformMatrix4fv(uniformView, 1, GL_FALSE,
				glm::value_ptr(glfwCamera.calculateViewMatrix()));
			int i = 0;
			for (RectangularPrism* prism : prisms) {
				if (i == 5) {
					shaders[1]->useShader();
					uniformModel = shaders[1]->getModelLocation();
					uniformProjection = shaders[1]->getProjectionLocation();
					uniformView = shaders[1]->getViewLocation();
					uniformScaleMatrix = shaders[1]->getScaleMatrixLocation();
					uniformAmbientColor = shaders[1]->getAmbientColorLocation();
					uniformAmbientIntensity = shaders[1]->getAmbientIntensityLocation();
					simpleLight.useLight(uniformAmbientIntensity, uniformAmbientColor);
					glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
						glm::value_ptr(glfwWindow->getProjection()));
					glUniformMatrix4fv(uniformView, 1, GL_FALSE,
						glm::value_ptr(glfwCamera.calculateViewMatrix()));

					prism->setRotationAngle(sinArgument * toRadians);
					prism->getMesh()->mapBufferRangeVertexColorData();
				}

				prism->transform();

				// access uniform variables for shaders
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
					glm::value_ptr(prism->getModelMatrix()));
				glUniformMatrix4fv(uniformScaleMatrix, 1, GL_FALSE,
					glm::value_ptr(prism->scaleTexture()));

				prism->getTexture()->useTexture();
				prism->renderMesh();

				++i;
			}

			

			//// Use Textures
			//prisms[0]->getTexture().useTexture();
			//prisms[0]->renderMesh();



			//// PRISM 1
			//prisms[1]->setTranslationVector(glm::vec3(0.0f, 3.0f, 4.5f));
			//prisms[1]->setRotationAngle(0.0f);
			//prisms[1]->setRotationVector(glm::vec3(0.0f, 1.0f, 0.0f));
			//prisms[1]->setScalingVector(glm::vec3(10.0f, 5.0f, 1.0f));
			//prisms[1]->transform();
			//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, 
			//	glm::value_ptr(prisms[1]->getModelMatrix()));

			//// Use Textures
			//prisms[1]->getTexture().useTexture();
			//prisms[1]->renderMesh();

			//// Make a floor
			//model = glm::mat4(1);
			//model = glm::translate(model,
			//	glm::vec3(0.0f, 0.0f, 0.0f));
			//model = glm::rotate(model, 0.0f,
			//	glm::vec3(0.0f, 1.0f, 0.0f));
			//model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
			//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			//grassTexture.useTexture();
			//meshes[2]->renderMesh();

			//// Make a floor
			//model = glm::mat4(1);
			//model = glm::translate(model,
			//	glm::vec3(4.5f, 3.0f, 0.0f));
			//model = glm::rotate(model, 0.0f,
			//	glm::vec3(0.0f, 1.0f, 0.0f));
			//model = glm::scale(model, glm::vec3(1.0f, 5.0f, 8.0f));
			//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			//brickTexture.useTexture();
			//meshes[3]->renderMesh();

			//// Make a floor
			//model = glm::mat4(1);
			//model = glm::translate(model,
			//	glm::vec3(-4.5f, 3.0f, 0.0f));
			//model = glm::rotate(model, 0.0f,
			//	glm::vec3(0.0f, 1.0f, 0.0f));
			//model = glm::scale(model, glm::vec3(1.0f, 5.0f, 8.0f));
			//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			//brickTexture.useTexture();
			//meshes[4]->renderMesh();

			//model = glm::mat4(1);
			//model = glm::translate(model,
			//	glm::vec3(0.0f, 3.0f, 0.0f));
			//model = glm::rotate(model, sinArgument * toRadians,
			//	glm::vec3(0.0f, 1.0f, 0.0f));
			//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			//dirtTexture.useTexture();
			//meshes[5]->renderMesh();

			glUseProgram(0); // Unbinds the shader

			// OpenGL has two buffers/frames
			// One the user sees, and the one that is being drawn
			// We see the next frame by switching the address the pointer
			// Is pointing to
			glfwWindow->swapBuffers();


		}
	}
};