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
#include "Skybox.hpp"

// for starting the world
#include "WorldLoader.cpp"

//#include "MeshCollider.hpp"

class GlfwGame {
private:
	std::vector<Material> materials;
	std::vector<Shader*> shaders;
	std::vector<RectangularPrism> prisms;

	std::vector<RectangularPrism> prisms2;
	std::vector <std::string> skyboxFaces;
	//std::vector<PointLight> pointLights;
	//std::vector<SpotLight> spotLights;

	//SpotLight spotLights[MAX_SPOT_LIGHTS];
	//PointLight pointLights[MAX_POINT_LIGHTS];
	SpotLight *spotLights;
	PointLight *pointLights;

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
		uniformShininess,
		uniformOmniLightPos,
		uniformFarPlane;

	GLfloat deltaTime, lastTime;

	GlfwWindow *glfwWindow;
	GlfwCamera glfwCamera;
	CameraBall cameraBall;

	Texture *dirtTexture, *brickTexture, *grassTexture, *boxTexture,
		*rustTexture, *plainTexture;
	
	Material shinyMaterial, dullMaterial, superShinyMaterial;

	DirectionalLight directionalLight;
	glm::mat4 lightTransform;

	unsigned int pointLightCount;

	PointLight singlePointLight;

	GLenum error;

	Skybox skybox;
	MeshGroup cat;

	Shader *directionalShadowShader, shadowPass;

	float incrementArgument;
	WorldLoader worldLoader;
	//std::vector<RectangularPrism> *prismsPtr;
public:
	GlfwGame() {
		shaders = std::vector<Shader*>();
		//prisms = std::vector<RectangularPrism>();
		//pointLights = std::vector<PointLight>();
		//spotLights = std::vector<SpotLight>();
		
		

		uniformModel = 0;
		uniformProjection = 0;
		uniformView = 0;

		deltaTime = 0.0f;
		lastTime = 0.0f;

		glfwWindow = NULL;
		
		lightTransform = glm::mat4(1);
		incrementArgument = 1.0f;
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

		glfwWindow = new GlfwWindow(800, 600);
		glfwWindow->initialize();

		WorldLoader::load();
		prisms = WorldLoader::m_prisms;
		directionalLight = WorldLoader::m_directionalLight;
		spotLights = WorldLoader::m_spotLights;
		pointLights = WorldLoader::m_pointLights;
		skybox = WorldLoader::m_skybox;
		shaders = WorldLoader::m_shaders;

		glfwCamera = GlfwCamera();

		shinyMaterial = WorldLoader::m_materials[0];
		dullMaterial = WorldLoader::m_materials[1];
		superShinyMaterial = WorldLoader::m_materials[2];

		cat = WorldLoader::m_meshGroups[0];
	}

	void renderCat() {
		glm::mat4 model1 = glm::mat4(1);
		model1 = glm::translate(model1, glm::vec3(10.0f, 0.5f, 22.5f));
		//model1 = glm::translate(model1, glm::vec3(7.0f, 0.5f, 1.5f));
		model1 = glm::rotate(model1, glm::radians(-90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f));

		incrementArgument += 0.05f;
		model1 = glm::rotate(model1, glm::radians(incrementArgument), glm::vec3(0.0f, 0.0f, 1.0f));
		model1 = glm::translate(model1, glm::vec3(-2.0f, 0.0f, 0.0f));

		model1 = glm::scale(model1, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
			glm::value_ptr(model1));
		cat.render();
	}

	void renderScene() {
		glm::vec3 flashPos = glfwCamera.getCameraPosition();
		flashPos.y = flashPos.y - 0.3f;
		spotLights[3].setFlash(flashPos,
			glfwCamera.getCameraFront());

		////////////////////////////////////
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
		glm::mat4 model1 = glm::mat4(1);
		for (auto &prism : prisms) {
			prism.transform();

			if (i < 5) {
				prism.setRotationAngle(0.005f * incrementArgument);
			}

			// access uniform variables for shaders
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
				glm::value_ptr(prism.getModelMatrix()));


			glUniformMatrix4fv(uniformScaleMatrix, 1, GL_FALSE,
				glm::value_ptr(prism.scaleTexture()));

			prism.getTexture()->useTexture();


			if (i == 20) {
				superShinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
			} else {
				dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
			}
			

			prism.renderMesh();
			++i;
		}
		renderCat();

	}

	void directionalShadowMapPass() {
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "directionalShadowMapPass() error: " << error << std::endl;
		} 

		directionalLight.getShadowMap()->bindToFrameBuffer();

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "directionalShadowMapPass() error: " << error 
				<< "When binding to framebuffer" << std::endl;
		}

		glClear(GL_DEPTH_BUFFER_BIT);

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "directionalShadowMapPass() error: " << error 
				<< " When using glClear() " << std::endl;
		}

		shaders[1]->useShader();

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "directionalShadowMapPass() error: " << error 
				<< " When calling useShader() " << std::endl;
		}

		uniformModel = shaders[1]->getModelLocation();
		lightTransform = directionalLight.calculateLightTransform();
		shaders[1]->setDirectionalLightTransform(
			lightTransform);
		shaders[1]->validate();

		for (auto &prism : prisms) {
			prism.transform();
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
				glm::value_ptr(prism.getModelMatrix()));

			prism.renderMesh();

		}

		renderCat();

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "directionalShadowMapPass() error: " << error
				<< " When drawing the scene " << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "directionalShadowMapPass() error: " << error
				<< " When unbinding framebuffer " << std::endl;
		}
	}

	void omniShadowMapPass(PointLight *light, int i) {

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error << std::endl;
		} 

		light->getShadowMap()->bindToFrameBuffer();

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error 
				<< " at bindToFrameBuffer() " << std::endl;
		}

		glClear(GL_DEPTH_BUFFER_BIT);

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error
				<< " at glClear() " << std::endl;
		}

		Shader *omniShader = shaders[2];
		omniShader->useShader();

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error
				<< " at useShader() " << std::endl;
		}

		uniformModel = omniShader->getModelLocation();
		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error
				<< " at getModelLocation()" << std::endl;
		}

		uniformFarPlane = omniShader->getFarPlaneLocation();
		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error
				<< " at getFarPlaneLocation()" << std::endl;
		}

		uniformOmniLightPos = omniShader->getOmniLightPositionLocation();


		// translate cave light
		// glm::vec3(31.0f, 2.0f, 10.0f)
		if (i == 0) {
			glm::vec3 lPos = light->getPosition();
			glm::vec3 pos = glm::vec3(
				lPos.x + 0.02f * sin(glm::radians(incrementArgument)), 
				lPos.y, 
				lPos.z + 0.02f * cos(glm::radians(incrementArgument)));
			light->setPosition(pos);
		} else if (i == MAX_POINT_LIGHTS) {
			glm::vec3 lPos = light->getPosition();
			glm::vec3 pos = glm::vec3(
				lPos.x + 0.05f * sin(glm::radians(incrementArgument)),
				//lPos.x,
				lPos.y,
				//lPos.z + 0.02f * cos(glm::radians(incrementArgument)));
				lPos.z);
			light->setPosition(pos);
		}

		glUniform3f(uniformOmniLightPos, light->getPosition().x,
			light->getPosition().y, light->getPosition().z);
		
		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error
				<< " at uniformOmniLightPos" << std::endl;
		}
		
		glUniform1f(uniformFarPlane, light->getFarPlane());

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error
				<< " at uniformFarPlane" << std::endl;
		}

		omniShader->setLightMatrices(light->calculateLightTransform());

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error
				<< " at setLightMatrices" << std::endl;
		}


		shaders[2]->validate();
		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error
				<< " when validating shader" << std::endl;
		}


		for (auto &prism : prisms) {
			prism.transform();
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
				glm::value_ptr(prism.getModelMatrix()));

			prism.renderMesh();

			error = glGetError();
			if (error != GL_NO_ERROR) {
				std::cout << "omniShadowMapPass() error: " << error
					<< " when rendering prism: " << &prism << std::endl;
			}

		}

		renderCat();

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error
				<< " when rendering cat " << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "omniShadowMapPass() error: " << error
				<< " at unbinding framebuffer " << std::endl;
		}
	}

	void renderPass() {


		glfwWindow->resetViewport();
		// Clear Window
		// Used to wipe the frame to a "blank canvas"
		// Otherwise we could end up with a lot of artifacts
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Specify we want to clear the color data in the buffer
		// we can specify we want to clear the depth data
		// Use the bitwise OR '|' to clear both color buffer bit
		// and the depth buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox.drawSkybox(glfwCamera.calculateViewMatrix(), 
			glfwWindow->getProjection());

		// Use the shader program
		// This will grab the shader with this ID
		// After we have created a shader, compiled it, checked it
		// 
		// In production, we would have a bunch of shaders,
		// Draw what we need to with one shader, swap it out,
		// Draw whatever we need with the next one, etc.
		shaders[0]->useShader();
		uniformModel = shaders[0]->getModelLocation();
		uniformProjection = shaders[0]->getProjectionLocation();
		uniformView = shaders[0]->getViewLocation();
		uniformScaleMatrix = shaders[0]->getScaleMatrixLocation();
		uniformEyePosition = shaders[0]->getEyePositionLocation();
		uniformSpecularIntensity = shaders[0]->getSpecularIntensityLocation();
		uniformShininess = shaders[0]->getShininessLocation();

		// sets back to render viewport for the render pass


		shaders[0]->setDirectionalLight(&directionalLight);

		// we are only working with one light right now
		shaders[0]->setPointLights(pointLights, 3, 3, 0);
		shaders[0]->setSpotLights(spotLights, 4, 
			3 + MAX_POINT_LIGHTS, MAX_POINT_LIGHTS);

		//shaders[0]->setPointLights(&singlePointLight, 1, 2, 0);
		
		lightTransform = directionalLight.calculateLightTransform();
		shaders[0]->setDirectionalLightTransform(
			lightTransform);

		directionalLight.getShadowMap()->read(GL_TEXTURE2);
		shaders[0]->setTexture(1);
		shaders[0]->setDirectionalShadowMap(2);

		shaders[0]->validate();

		renderScene();
	}


	void runGame() {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		//glEnable(GL_DEPTH_TEST);


		float sinArg = 0.25f;

		while (!glfwWindow->getShouldClose()) {
			GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter?
			deltaTime = now - lastTime; // 1000*(now - lastTime) / SDL_GetPerformanceFrequency();
			lastTime = now;

			// update collisions

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

			glm::vec3 dir = directionalLight.getDirection();
			directionalLight.setDirection(dir);


			directionalShadowMapPass();
			for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
				omniShadowMapPass(&pointLights[i], i);
			}

			for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
				omniShadowMapPass(&spotLights[i], MAX_POINT_LIGHTS + i);
			}

			renderPass();
			//testRenderPath();
			//glUseProgram(0); // Unbinds the shader

			// OpenGL has two buffers/frames
			// One the user sees, and the one that is being drawn
			// We see the next frame by switching the address the pointer
			// Is pointing to
			glfwWindow->swapBuffers();
			sinArg += 0.1f;

		}
	}

	void depthData() {
		int width = directionalLight.getShadowMap()->getShadowWidth();
		int height = directionalLight.getShadowMap()->getShadowHeight();

		GLfloat *depthData = new GLfloat[width * height];
		glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT,
			GL_FLOAT, depthData);

		// Analyze the depth data to check for meaningful information
		bool hasRealInformation = false;
		for (int i = 0; i < width * height; ++i) {
			// Check if any of the depth values are within a meaningful range
			if (depthData[i] > 0.0 && depthData[i] < 1.0) {
				hasRealInformation = true;
				//std::cout << "has data at: " << i << " with " 
				//	<< depthData[i] << std::endl;
				//break;
			}
		}
	}
};

