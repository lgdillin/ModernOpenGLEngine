#pragma once

#include <iostream>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

#include <assimp/Importer.hpp>


#include "GlfwWindow.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "GlfwCamera.hpp"
#include "Texture.hpp"
#include "RectangularPrism.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "Material.hpp"
#include "CameraBall.hpp"
#include "SpotLight.hpp"
#include "MeshGroup.hpp"
//#include "MeshCollider.hpp"

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
		uniformAmbientColor,
		uniformDirection,
		uniformDiffuseIntensity, 
		uniformEyePosition,
		uniformSpecularIntensity,
		uniformShininess;

	GLfloat deltaTime, lastTime;

	GlfwWindow *glfwWindow;
	GlfwCamera glfwCamera;
	CameraBall cameraBall;

	Texture *dirtTexture, *brickTexture, *grassTexture, *boxTexture,
		*rustTexture, *plainTexture;
	
	Material shinyMaterial, dullMaterial, superShinyMaterial;

	DirectionalLight directionalLight;
	PointLight pointLights[MAX_POINT_LIGHTS];
	SpotLight spotLights[MAX_SPOT_LIGHTS];

	unsigned int pointLightCount;

	GLenum error;

	MeshGroup plant;

	//MeshCollider MeshCollider;

	//Assimp::Importer *importer;

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
		//cameraBall = CameraBall();
	}

	void foundCollisionAABB2(RectangularPrism *rp) {
		glm::vec3 cameraPosition = cameraBall.getCamera()->getCameraPosition();

		GLfloat x = glm::max(
			rp->getXMinBound(), glm::min(cameraPosition.x, rp->getXMaxBound()));

		GLfloat y = glm::max(
			rp->getYMinBound(), glm::min(cameraPosition.y, rp->getYMaxBound()));

		GLfloat z = glm::max(
			rp->getZMinBound(), glm::min(cameraPosition.z, rp->getZMaxBound()));

		GLfloat distanceY = glm::sqrt(
			(x - cameraPosition.x) * (x - cameraPosition.x)
			+ (y - cameraPosition.y) * (y - cameraPosition.y)
			+ (z - cameraPosition.z) * (z - cameraPosition.z)
		);

		//std::cout << " x " << x << " - " << cameraPosition.x 
		//	<< " = " << (x - cameraPosition.x) << std::endl;

		//std::cout << " y " << y << " - " << cameraPosition.y
		//	<< " = " << (y - cameraPosition.y) << std::endl;

		//std::cout << " z " << x << " - " << cameraPosition.z
		//	<< " = " << (z - cameraPosition.z) << std::endl;

		glm::vec3 lastLegalPosition = glm::vec3(x, y, z);
		if (distanceY < cameraBall.getEyeHeight()) {
			glm::vec3 newPosition = cameraBall.getCamera()->getLastPosition();
			cameraBall.getCamera()->setLastPosition(cameraPosition);
			cameraBall.getCamera()
				->setCurrentPosition(newPosition);
		}
	}




	// Define a callback function
	void DebugMessageCallback(GLenum source, GLenum type, GLuint id, 
		GLenum severity, GLsizei length, const GLchar* message, 
		const void* userParam
	){
		// Print the debug message to the console
		std::cout << "OpenGL Debug Message: " << message << std::endl;
	}

	// Have some stuff to get the loop started
	void init() {
		glEnable(GL_DEBUG_OUTPUT);
		//glDebugMessageCallback(DebugMessageCallback, nullptr);
		// Initialize GLFW
		//Assimp::Importer importer = Assimp::Importer();

		glfwWindow = new GlfwWindow(800, 600);
		glfwWindow->initialize();

		glfwCamera = GlfwCamera();
		cameraBall = CameraBall(&glfwCamera);

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

		directionalLight = DirectionalLight(
			glm::vec3(1.0f, 3.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			0.1f,
			0.2f
		);

		pointLights[0] = PointLight(
			glm::vec3(8.0f, 0.5f, -0.5f),
			glm::vec3(1.0f, 1.0f, 0.0f)
		);
		pointLights[1] = PointLight(
			glm::vec3(2.0f, 2.0f, 5.0f),
			glm::vec3(0.5f, 1.0f, 0.0f)
		);
		pointLights[2] = PointLight(
			glm::vec3(10.0f, 6.0f, 22.5f),
			glm::vec3(1.0f, 0.5f, 0.0f)
		);
		
		spotLights[0] = SpotLight(glm::vec3(0.0f, 10.0f, 0.0f));
		spotLights[0].setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
		spotLights[0].setEdge(20.0f);
		spotLights[0].setColor(glm::vec3(1.0f, 1.0f, 1.0f));

		spotLights[1] = SpotLight(glm::vec3(12.0f, 4.0f, 5.0f));
		spotLights[1].setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
		spotLights[1].setConstantFactor(0.02f);
		spotLights[1].setLinearFactor(0.006f);
		spotLights[1].setEdge(30.0f);
		spotLights[1].setColor(glm::vec3(1.0f, 0.0f, 1.0f));

		spotLights[2] = SpotLight(glm::vec3(10.0f, 4.0f, 14.5f));
		spotLights[2].setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
		spotLights[2].setEdge(65.0f);
		spotLights[2].setColor(glm::vec3(0.0f, 1.0f, 1.0f));

		// Flashlight
		spotLights[3] = SpotLight(glfwCamera.getCameraPosition());
		spotLights[3].setDirection(glfwCamera.getCameraFront());
		spotLights[3].setEdge(10.0f);
		spotLights[3].setColor(glm::vec3(1.0f, 1.0f, 1.0f));


		brickTexture = new Texture("resources/brick.png");
		brickTexture->loadTextureAlphaOption(true);
		dirtTexture = new Texture("resources/dirt.png");
		dirtTexture->loadTextureAlphaOption(true);
		grassTexture = new Texture("resources/grass.png");
		grassTexture->loadTextureAlphaOption(true);
		boxTexture = new Texture("resources/box.png");
		boxTexture->loadTextureAlphaOption(true);
		rustTexture = new Texture("resources/rusty.png");
		rustTexture->loadTextureAlphaOption(true);
		plainTexture = new Texture("resources/plain.png");
		plainTexture->loadTextureAlphaOption(true);

		std::string vShader = "./vertexShader.vert";
		std::string fShader = "./fragmentShader.frag";
		std::string fShader1 = "./fragmentShaderColor.frag";

		Shader *shader1 = new Shader();
		shader1->createFromFile(vShader, fShader);
		shaders.push_back(shader1);

		Shader *shader2 = new Shader();
		shader2->createFromFile(vShader, fShader1);
		shaders.push_back(shader2);

		shinyMaterial = Material(1.0f, 32);
		//dullMaterial = Material(0.4f, 8);
		dullMaterial = Material(1.0f, 64);

		superShinyMaterial = Material(1.0f, 128);

		plant = MeshGroup();
		plant.load("resources/12221_Cat_v1_l3.obj");

		// floor1
		addNewBrush(
			"green",
			grassTexture,
			glm::vec3(0.0f, 0.0f, 0.0f),
			90.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(10.0f, 10.0f, 1.0f));

		// floor 2
		addNewBrush(
			"green",
			grassTexture,
			glm::vec3(10.0f, 0.0f, 0.0f),
			90.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(10.0f, 10.0f, 1.0f));

		// floor 3
		addNewBrush(
			"green",
			grassTexture,
			glm::vec3(10.0f, 0.0f, 10.0f),
			90.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(10.0f, 10.0f, 1.0f));

		// floor 4
		addNewBrush(
			"green",
			dirtTexture,
			glm::vec3(10.0f, 0.0f, 22.5f),
			90.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(15.0f, 15.0f, 1.0f));

		// house ceiling
		addNewBrush(
			"green",
			rustTexture,
			glm::vec3(10.0f, 9.0f, 22.5f),
			90.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(15.0f, 15.0f, 1.0f));

		// short right wall room 1
		addNewBrush(
			"red",
			brickTexture,
			glm::vec3(2.5f, 3.0f, 4.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(15.0f, 5.0f, 1.0f)
		);

		// long left wall room 1
		addNewBrush(
			"blue",
			brickTexture,
			glm::vec3(5.0f, 3.0f, -4.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(20.0f, 5.0f, 1.0f)
		);

		// back wall room 1
		addNewBrush(
			"red",
			brickTexture,
			glm::vec3(-4.5f, 3.0f, 0.0),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(8.0f, 5.0f, 1.0f)
		);

		// far wall room 2
		addNewBrush(
			"red",
			brickTexture,
			glm::vec3(14.5f, 3.0f, 0.5f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(9.0f, 5.0f, 1.0f)
		);

		// right wall room 3
		addNewBrush(
			"red",
			brickTexture,
			glm::vec3(5.5f, 3.0f, 10.0f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(10.0f, 5.0f, 1.0f)
		);

		// left wall room 3
		addNewBrush(
			"red",
			brickTexture,
			glm::vec3(14.5f, 3.0f, 10.0f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(10.0f, 5.0f, 1.0f)
		);

		// door partition
		addNewBrush(
			"red",
			brickTexture,
			glm::vec3(4.5f, 3.0f, 3.25f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.5f, 5.0f, 1.0f)
		);

		// door partition
		addNewBrush(
			"red",
			brickTexture,
			glm::vec3(4.5f, 3.0f, -3.25f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.5f, 5.0f, 1.0f)
		);

		// house left door partition
		addNewBrush(
			"blue",
			brickTexture,
			glm::vec3(15.0f, 3.0f, 15.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(5.0f, 5.0f, 1.0f)
		);

		// house right door partition
		addNewBrush(
			"blue",
			brickTexture,
			glm::vec3(5.0f, 3.0f, 15.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(5.0f, 5.0f, 1.0f)
		);

		// house door header
		addNewBrush(
			"blue",
			brickTexture,
			glm::vec3(10.0f, 7.0f, 15.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(15.0f, 3.0f, 1.0f)
		);

		// house back wall
		addNewBrush(
			"blue",
			brickTexture,
			glm::vec3(10.0f, 4.5f, 29.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(15.0f, 8.0f, 1.0f)
		);

		// house left wall
		addNewBrush(
			"red",
			brickTexture,
			glm::vec3(17.0f, 4.5f, 22.5f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(13.0f, 8.0f, 1.0f)
		);

		// house right wall
		addNewBrush(
			"red",
			brickTexture,
			glm::vec3(3.0f, 4.5f, 22.5f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(13.0f, 8.0f, 1.0f)
		);

		addNewBrush(
			"white",
			boxTexture,
			glm::vec3(0.0f, 3.0f, 0.0f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f)
		);



		addNewBrush(
			"white",
			plainTexture,
			glm::vec3(10.0f, 3.0f, 22.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 2.0f)
		);

	}

	void findShortestPath() {

	}

	void addNewBrush(
		std::string color,
		Texture* texture,
		glm::vec3 translation,
		float rotationAngle,
		glm::vec3 rotVector,
		glm::vec3 scale
	) {
		RectangularPrism *rp = new RectangularPrism();
		rp->initialize();
		rp->pickColor(color);
		rp->setTexture(texture);
		rp->setTranslationVector(translation);
		rp->setRotationAngle(glm::radians(rotationAngle));
		rp->setRotationVector(rotVector);
		rp->setScalingVector(scale);
		prisms.push_back(rp);
	}


	void runGame() {
		bool directionToggle = true;

		const float toRadians = 3.14159265f / 180.0f;
		float triangleOffset = 0.0f;
		float triangleMaximumOffset = 0.5f;
		float triangleIncrementalValue = 0.0002f;

		float incrementArgument = 1.0f;
		float sinArgument = 0.0f;
		float sinColor = 0.0f;

		while (!glfwWindow->getShouldClose()) {
			GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter?
			deltaTime = now - lastTime; // 1000*(now - lastTime) / SDL_GetPerformanceFrequency();
			lastTime = now;

			// update collisions
			//cameraBall.applyGravity();

			// Get and handle user input events
			glfwPollEvents();
			glfwCamera.keyControl(glfwWindow->getKeys(), 
				glfwWindow->getKeyHit(), deltaTime);
			glfwCamera.mouseControl(glfwWindow->getMouseDeltaX(),
				glfwWindow->getMouseDeltaY());

			// toggle flashlight
			if (glfwCamera.getToggleFlashlight()) {
				spotLights[3].setColor(glm::vec3(0.0f, 0.0f, 0.0f));
			} else {
				spotLights[3].setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			}

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
			//uniformAmbientColor = shaders[0]->getColorLocation();
			//uniformAmbientIntensity = shaders[0]->getAmbientIntensityLocation();
			//uniformDiffuseIntensity = shaders[0]->getDiffuseIntensity();
			//uniformDirection = shaders[0]->getDirectionLocation();
			uniformEyePosition = shaders[0]->getEyePositionLocation();
			uniformSpecularIntensity = shaders[0]->getSpecularIntensityLocation();
			uniformShininess = shaders[0]->getShininessLocation();

			//directionalLight.useLight(uniformAmbientIntensity, uniformAmbientColor, 
			//	uniformDiffuseIntensity, uniformDirection);
			//error = glGetError(); std::cout << error;
			shaders[0]->setDirectionalLight(&directionalLight);
			//error = glGetError(); std::cout << error;
			shaders[0]->setPointLights(pointLights, 3);
			//error = glGetError(); std::cout << error;
			shaders[0]->setSpotLights(spotLights, 4);

			glm::vec3 flashPos = glfwCamera.getCameraPosition();
			flashPos.y = flashPos.y - 0.3f;
			spotLights[3].setFlash(flashPos,
				glfwCamera.getCameraFront());

			sinArgument += 0.03f;
			incrementArgument += 0.05f;
			sinColor += .25f;


			// render/update loop
			glUniform3f(uniformEyePosition,
				glfwCamera.getCameraPosition().x,
				glfwCamera.getCameraPosition().y,
				glfwCamera.getCameraPosition().z);
			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
				glm::value_ptr(glfwWindow->getProjection()));
			glUniformMatrix4fv(uniformView, 1, GL_FALSE,
				glm::value_ptr(glfwCamera.calculateViewMatrix()));
			int i = 0;
			for (RectangularPrism* prism : prisms) {

				if (i == 90) {
					shaders[1]->useShader();
					uniformModel = shaders[1]->getModelLocation();
					uniformProjection = shaders[1]->getProjectionLocation();
					uniformView = shaders[1]->getViewLocation();
					uniformScaleMatrix = shaders[1]->getScaleMatrixLocation();
					//uniformAmbientColor = shaders[1]->getColorLocation();
					//uniformAmbientIntensity = shaders[1]->getAmbientIntensityLocation();
					//uniformDiffuseIntensity = shaders[1]->getDiffuseIntensity();
					//uniformDirection = shaders[1]->getDirectionLocation();
					std::cout << "this should not execute";
					//directionalLight.useLight(uniformAmbientIntensity, uniformAmbientColor,
					//	uniformDiffuseIntensity, uniformDirection);
					glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
						glm::value_ptr(glfwWindow->getProjection()));
					glUniformMatrix4fv(uniformView, 1, GL_FALSE,
						glm::value_ptr(glfwCamera.calculateViewMatrix()));

					prism->setRotationAngle(sinArgument * toRadians);
					prism->getMesh()->mapBufferRangeVertexColorData();
				}

				prism->transform();
				//foundCollisionAABB2(prism);

				// access uniform variables for shaders
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
					glm::value_ptr(prism->getModelMatrix()));
				glUniformMatrix4fv(uniformScaleMatrix, 1, GL_FALSE,
					glm::value_ptr(prism->scaleTexture()));

				//prism->printVertexData();

				prism->getTexture()->useTexture();

				if (i == 20) {
					superShinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
				} else {
					dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
				}
				prism->renderMesh();
				++i;
			}

			glm::mat4 model1 = glm::mat4(1);
			model1 = glm::translate(model1, glm::vec3(10.0f, 0.5f, 22.5f));
			//model1 = glm::translate(model1, glm::vec3(3.0f, 0.0f, 0.0f));
			model1 = glm::rotate(model1, glm::radians(-90.0f),
				glm::vec3(1.0f, 0.0f, 0.0f));

			
			model1 = glm::rotate(model1, glm::radians(incrementArgument), glm::vec3(0.0f, 0.0f, 1.0f));
			model1 = glm::translate(model1, glm::vec3(-2.0f, 0.0f, 0.0f));

			model1 = glm::scale(model1, glm::vec3(0.03f, 0.03f, 0.03f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
				glm::value_ptr(model1));

			plant.render();

			cameraBall.getCamera()->setLastPosition(
				cameraBall.getCamera()->getCameraPosition());

			glUseProgram(0); // Unbinds the shader

			// OpenGL has two buffers/frames
			// One the user sees, and the one that is being drawn
			// We see the next frame by switching the address the pointer
			// Is pointing to
			glfwWindow->swapBuffers();


		}
	}
};