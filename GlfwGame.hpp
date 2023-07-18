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
	std::vector<RectangularPrism> prisms;

	std::vector<RectangularPrism> prisms2;
	//std::vector<PointLight> pointLights;
	//std::vector<SpotLight> spotLights;

	SpotLight spotLights[MAX_SPOT_LIGHTS];
	PointLight pointLights[MAX_POINT_LIGHTS];

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
	glm::mat4 lightTransform;

	unsigned int pointLightCount;

	GLenum error;

	MeshGroup plant;

	Shader *directionalShadowShader, shadowPass;

	float incrementArgument;

public:
	GlfwGame() {
		meshes = std::vector<Mesh*>();
		shaders = std::vector<Shader*>();
		prisms = std::vector<RectangularPrism>();
		//pointLights = std::vector<PointLight>();
		//spotLights = std::vector<SpotLight>();

		uniformModel = 99;
		uniformProjection = 0;
		uniformView = 0;

		deltaTime = 0.0f;
		lastTime = 0.0f;

		glfwWindow = NULL;
		//cameraBall = CameraBall();
		
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
		//glDebugMessageCallback(DebugMessageCallback, nullptr);

		glfwWindow = new GlfwWindow(800, 600);
		glfwWindow->initialize();

		glfwCamera = GlfwCamera();
		cameraBall = CameraBall(&glfwCamera);



		directionalLight = DirectionalLight(
			glm::vec3(-20.0f, 25.0f, 8.0f),
			glm::vec3(1.0f, 0.9f, 0.9f),
			0.1f,
			0.3f
		);
		directionalLight.initShadowMap();

		pointLights[0] = PointLight(
			glm::vec3(8.0f, 0.5f, -0.5f),
			glm::vec3(1.0f, 1.0f, 0.0f)
		);
		pointLights[1] = PointLight(
			glm::vec3(2.0f, 2.0f, 5.0f),
			glm::vec3(0.5f, 1.0f, 0.0f)
		);
		pointLights[2] = PointLight(
			glm::vec3(10.0f, 5.0f, 22.5f),
			glm::vec3(1.0f, 0.5f, 0.0f)
		);
		pointLights[2].setIntensity(0.4f, 0.7f);
		

		for (auto &pl : pointLights) {
			//pl.initShadowMap();
		}
		
		SpotLight sp1 = SpotLight(glm::vec3(-3.0f, 10.0f, 0.0f));
		sp1.setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
		sp1.setEdge(20.0f);
		sp1.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		spotLights[0] = sp1;

		SpotLight sp2 = SpotLight(glm::vec3(12.0f, 4.0f, 5.0f));
		sp2.setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
		sp2.setConstantFactor(0.02f);
		sp2.setLinearFactor(0.006f);
		sp2.setEdge(30.0f);
		sp2.setColor(glm::vec3(1.0f, 0.0f, 1.0f));
		spotLights[1] = sp2;

		SpotLight sp3 = SpotLight(glm::vec3(10.0f, 3.0f, 14.5f));
		sp3.setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
		sp3.setEdge(65.0f);
		sp3.setColor(glm::vec3(0.0f, 1.0f, 1.0f));
		spotLights[2] = sp3;

		// Flashlight
		SpotLight sp4 = SpotLight(glfwCamera.getCameraPosition());
		sp4.setDirection(glfwCamera.getCameraFront());
		sp4.setEdge(10.0f);
		sp4.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		spotLights[3] = sp4;

		for (auto &sl : spotLights) {
			//sl.initShadowMap();
		}


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
		
		std::string vShader1 = "vDirectionalShadowMap.vert";
		std::string fShader1 = "fDirectionalShadowMap.frag";

		Shader *shader1 = new Shader();
		shader1->createFromFile(vShader, fShader);
		shaders.push_back(shader1);

		Shader *shader2 = new Shader();
		shader2->createFromFile(vShader1, fShader1);
		shaders.push_back(shader2);
		directionalShadowShader = shaders[1];

		//shadowPass.createFromFile("shadowPass.vert", "shadowPass.frag");

		shinyMaterial = Material(1.0f, 32);
		//dullMaterial = Material(0.4f, 8);
		dullMaterial = Material(1.0f, 64);

		superShinyMaterial = Material(1.0f, 128);

		plant = MeshGroup();
		plant.load("resources/12221_Cat_v1_l3.obj");

		// floor1
		addNewBrushS(
			"green",
			grassTexture,
			glm::vec3(0.0f, 0.0f, 0.0f),
			90.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(10.0f, 10.0f, 1.0f));

		// floor 2
		addNewBrushS(
			"green",
			grassTexture,
			glm::vec3(10.0f, 0.0f, 0.0f),
			90.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(10.0f, 10.0f, 1.0f));

		// floor 3
		addNewBrushS(
			"green",
			grassTexture,
			glm::vec3(10.0f, 0.0f, 10.0f),
			90.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(10.0f, 10.0f, 1.0f));

		// floor 4
		addNewBrushS(
			"green",
			dirtTexture,
			glm::vec3(10.0f, 0.0f, 22.5f),
			90.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(15.0f, 15.0f, 1.0f));

		// house ceiling
		addNewBrushS(
			"green",
			rustTexture,
			glm::vec3(10.0f, 9.0f, 22.5f),
			90.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(15.0f, 15.0f, 1.0f));

		// short right wall room 1
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(2.5f, 3.0f, 4.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(15.0f, 5.0f, 1.0f)
		);

		// long left wall room 1
		addNewBrushS(
			"blue",
			brickTexture,
			glm::vec3(5.0f, 3.0f, -4.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(20.0f, 5.0f, 1.0f)
		);

		// back wall room 1
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(-4.5f, 3.0f, 0.0),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(8.0f, 5.0f, 1.0f)
		);

		// far wall room 2
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(14.5f, 3.0f, 0.5f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(9.0f, 5.0f, 1.0f)
		);

		// right wall room 3
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(5.5f, 3.0f, 10.0f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(10.0f, 5.0f, 1.0f)
		);

		// left wall room 3
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(14.5f, 3.0f, 10.0f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(10.0f, 5.0f, 1.0f)
		);

		// door partition
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(4.5f, 3.0f, 3.25f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.5f, 5.0f, 1.0f)
		);

		// door partition
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(4.5f, 3.0f, -3.25f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.5f, 5.0f, 1.0f)
		);

		// house left door partition
		addNewBrushS(
			"blue",
			brickTexture,
			glm::vec3(15.0f, 3.0f, 15.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(5.0f, 5.0f, 1.0f)
		);

		// house right door partition
		addNewBrushS(
			"blue",
			brickTexture,
			glm::vec3(5.0f, 3.0f, 15.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(5.0f, 5.0f, 1.0f)
		);

		// house door header
		addNewBrushS(
			"blue",
			brickTexture,
			glm::vec3(10.0f, 7.0f, 15.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(15.0f, 3.0f, 1.0f)
		);

		// house back wall
		addNewBrushS(
			"blue",
			brickTexture,
			glm::vec3(10.0f, 4.5f, 29.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(15.0f, 8.0f, 1.0f)
		);

		// house left wall
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(17.0f, 4.5f, 22.5f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(13.0f, 8.0f, 1.0f)
		);

		// house right wall
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(3.0f, 4.5f, 22.5f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(13.0f, 8.0f, 1.0f)
		);

		addNewBrushS(
			"white",
			boxTexture,
			glm::vec3(0.0f, 3.0f, 0.0f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f)
		);



		addNewBrushS(
			"white",
			plainTexture,
			glm::vec3(10.0f, 3.0f, 22.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 2.0f)
		);

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
		//prisms.push_back(rp);
	}

	void addNewBrushS(
		std::string color,
		Texture *texture,
		glm::vec3 translation,
		float rotationAngle,
		glm::vec3 rotVector,
		glm::vec3 scale
	) {
		RectangularPrism rp = RectangularPrism();
		rp.initialize();
		rp.pickColor(color);
		rp.setTexture(texture);
		rp.setTranslationVector(translation);
		rp.setRotationAngle(glm::radians(rotationAngle));
		rp.setRotationVector(rotVector);
		rp.setScalingVector(scale);
		prisms.push_back(rp);
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
		for (auto &prism : prisms) {
			prism.transform();

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

		glm::mat4 model1 = glm::mat4(1);
		model1 = glm::translate(model1, glm::vec3(10.0f, 0.5f, 22.5f));
		model1 = glm::rotate(model1, glm::radians(-90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f));

		incrementArgument += 0.05f;
		model1 = glm::rotate(model1, glm::radians(incrementArgument), glm::vec3(0.0f, 0.0f, 1.0f));
		model1 = glm::translate(model1, glm::vec3(-2.0f, 0.0f, 0.0f));

		model1 = glm::scale(model1, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
			glm::value_ptr(model1));

		plant.render();
	}

	void directionalShadowMapPass() {
		directionalLight.getShadowMap()->bindToFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT);
		shaders[1]->useShader();

		uniformModel = shaders[1]->getModelLocation();
		lightTransform = directionalLight.calculateLightTransform();
		shaders[1]->setDirectionalLightTransform(
			lightTransform);

		for (auto &prism : prisms) {
			prism.transform();
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
				glm::value_ptr(prism.getModelMatrix()));

			prism.renderMesh();

		}

		glm::mat4 model1 = glm::mat4(1);
		model1 = glm::translate(model1, glm::vec3(10.0f, 0.5f, 22.5f));
		model1 = glm::rotate(model1, glm::radians(-90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f));

		incrementArgument += 0.05f;
		model1 = glm::rotate(model1, glm::radians(incrementArgument), glm::vec3(0.0f, 0.0f, 1.0f));
		model1 = glm::translate(model1, glm::vec3(-2.0f, 0.0f, 0.0f));

		model1 = glm::scale(model1, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
			glm::value_ptr(model1));

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
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
		shaders[0]->setPointLights(pointLights, 3);
		shaders[0]->setSpotLights(spotLights, 4);
		
		lightTransform = directionalLight.calculateLightTransform();
		shaders[0]->setDirectionalLightTransform(
			lightTransform);

		directionalLight.getShadowMap()->read(GL_TEXTURE1);
		shaders[0]->setTexture(0);
		shaders[0]->setDirectionalShadowMap(1);

		renderScene();
	}


	void testRenderPath() {
		directionalLight.getShadowMap()->bindToFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT);
		shaders[1]->useShader();

		uniformModel = shaders[1]->getModelLocation();
		lightTransform = directionalLight.calculateLightTransform();
		shaders[1]->setDirectionalLightTransform(
			lightTransform);

		for (auto &prism : prisms2) {
			prism.transform();
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
				glm::value_ptr(prism.getModelMatrix()));

			prism.renderMesh();

		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		depthData();
		//////////////////////////////////////////////////

		glfwWindow->resetViewport();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaders[0]->useShader();
		uniformModel = shaders[0]->getModelLocation();
		uniformProjection = shaders[0]->getProjectionLocation();
		uniformView = shaders[0]->getViewLocation();
		uniformScaleMatrix = shaders[0]->getScaleMatrixLocation();
		uniformEyePosition = shaders[0]->getEyePositionLocation();
		uniformSpecularIntensity = shaders[0]->getSpecularIntensityLocation();
		uniformShininess = shaders[0]->getShininessLocation();
		shaders[0]->setDirectionalLight(&directionalLight);
		shaders[0]->setPointLights(pointLights, 3);
		shaders[0]->setSpotLights(spotLights, 4);

		lightTransform = directionalLight.calculateLightTransform();
		shaders[0]->setDirectionalLightTransform(
			lightTransform);

		directionalLight.getShadowMap()->read(GL_TEXTURE1);
		shaders[0]->setTexture(0);
		shaders[0]->setDirectionalShadowMap(1);

		glm::vec3 flashPos = glfwCamera.getCameraPosition();
		flashPos.y = flashPos.y - 0.3f;
		spotLights[3].setFlash(flashPos,
			glfwCamera.getCameraFront());

		//////////////////////////////////////////////////////////
		glUniform3f(uniformEyePosition,
			glfwCamera.getCameraPosition().x,
			glfwCamera.getCameraPosition().y,
			glfwCamera.getCameraPosition().z);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
			glm::value_ptr(glfwWindow->getProjection()));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE,
			glm::value_ptr(glfwCamera.calculateViewMatrix()));

		int i = 0;
		for (auto& prism : prisms2) {
			prism.transform();
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
				glm::value_ptr(prism.getModelMatrix()));
			glUniformMatrix4fv(uniformScaleMatrix, 1, GL_FALSE,
				glm::value_ptr(prism.scaleTexture()));
			
			prism.getTexture()->useTexture();

			dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
			prism.renderMesh();

			++i;
		}
	}

	void runGame() {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		//glEnable(GL_DEPTH_TEST);

		// Testing shadows
		//addNewBrushS(
		//	"white",
		//	boxTexture,
		//	glm::vec3(0.0f, 0.0f, 0.0f),
		//	0.0f,
		//	glm::vec3(0.0f, 1.0f, 0.0f),
		//	glm::vec3(8.0f, 1.0f, 8.0f)
		//);

		//// Testing shadows
		//addNewBrushS(
		//	"white",
		//	boxTexture,
		//	glm::vec3(0.0f, 3.0f, 0.0f),
		//	45.0f,
		//	glm::vec3(1.0f, 0.0f, 1.0f),
		//	glm::vec3(1.0f, 1.0f, 1.0f)
		//);

		//// Testing shadows
		//addNewBrushS(
		//	"white",
		//	boxTexture,
		//	glm::vec3(2.0f, 3.0f, 3.0f),
		//	-45.0f,
		//	glm::vec3(1.0f, 0.0f, 1.0f),
		//	glm::vec3(1.0f, 5.0f, 1.0f)
		//);

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
			//dir.z += 0.007f * glm::sin(glm::radians(sinArg));
			//std::cout << glm::sin(glm::radians(sinArg)) << std::endl;
			directionalLight.setDirection(dir);


			directionalShadowMapPass();
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

