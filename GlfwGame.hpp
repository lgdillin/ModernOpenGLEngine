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
//#include "MeshCollider.hpp"

class GlfwGame {
private:
	std::vector<Mesh*> meshes;
	std::vector<Shader*> shaders;
	std::vector<RectangularPrism> prisms;

	std::vector<RectangularPrism> prisms2;
	std::vector < std::string> skyboxFaces;
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

		skyboxFaces.push_back("resources/skyboxes/ame_nebula/purplenebula_rt.tga");
		skyboxFaces.push_back("resources/skyboxes/ame_nebula/purplenebula_lf.tga");
		skyboxFaces.push_back("resources/skyboxes/ame_nebula/purplenebula_up.tga");
		skyboxFaces.push_back("resources/skyboxes/ame_nebula/purplenebula_dn.tga");
		skyboxFaces.push_back("resources/skyboxes/ame_nebula/purplenebula_bk.tga");
		skyboxFaces.push_back("resources/skyboxes/ame_nebula/purplenebula_ft.tga");
		skybox = Skybox(skyboxFaces);


		directionalLight = DirectionalLight(
			glm::vec3(5.0f, 28.0f, 25.0f),
			glm::vec3(1.0f, 0.9f, 0.9f),
			0.001f,
			0.1f
		);
		directionalLight.initShadowMap();

		pointLights[0] = PointLight( // cave light
			glm::vec3(28.5f, 2.0f, 10.0f),
			glm::vec3(1.0f, 0.5f, 1.0f)
		);
		pointLights[0].setIntensity(0.4, 0.8);
		pointLights[0].setFalloff(0.7f, 0.3f, 0.12f);

		pointLights[1] = PointLight(
			glm::vec3(8.0f, 2.5f, -0.5f),
			glm::vec3(1.0f, 1.0f, 0.0f)
		);
		pointLights[2] = PointLight(
			glm::vec3(10.0f, 6.0f, 22.5f),
			glm::vec3(1.0f, 0.5f, 0.0f)
		);
		pointLights[2].setIntensity(0.4f, 0.8f);
		//pointLights[0].setFalloff(0.7f, 0.3f, 0.12f);
		

		for (auto &pl : pointLights) {
			pl.initOmniShadowMap();
		}

		//singlePointLight = PointLight(
		//	glm::vec3(8.0f, 1.0f, 1.0f),
		//	glm::vec3(1.0f, 1.0f, 1.0f)
		//);
		//singlePointLight.initOmniShadowMap();
		
		SpotLight sp1 = SpotLight(glm::vec3(-1.0f, 12.0f, 0.0f));
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
		sp4.setIntensity(0.2f, 0.8f);
		spotLights[3] = sp4;

		for (auto &sl : spotLights) {
			sl.initOmniShadowMap();
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

		std::string vShader2 = "omniShadowMap.vert";
		std::string gShader2 = "omniShadowMap.geom";
		std::string fShader2 = "omniShadowMap.frag";

		Shader *shader1 = new Shader();
		shader1->createFromFile(vShader, fShader);
		shaders.push_back(shader1);

		Shader *shader2 = new Shader();
		shader2->createFromFile(vShader1, fShader1);
		shaders.push_back(shader2);
		directionalShadowShader = shaders[1];

		Shader *shader3 = new Shader();
		shader3->createFromFile(vShader2, gShader2, fShader2);
		shaders.push_back(shader3);

		shinyMaterial = Material(1.0f, 32);
		//dullMaterial = Material(0.4f, 8);
		dullMaterial = Material(1.0f, 64);

		superShinyMaterial = Material(1.0f, 128);

		plant = MeshGroup();
		plant.load("resources/12221_Cat_v1_l3.obj");

		// cave box 1
		addNewBrushS(
			"white",
			plainTexture,
			glm::vec3(36.0f, 3.0f, 10.0f),
			45.0f,
			glm::vec3(0.0f, 1.1f, 0.5f),
			glm::vec3(0.6f, 0.6f, 0.6f)
		);

		// cave box 2
		addNewBrushS(
			"white",
			plainTexture,
			glm::vec3(26.0f, 3.0f, 10.0f),
			0.0f,
			glm::vec3(0.5f, 0.7f, 0.0f),
			glm::vec3(0.6f, 0.6f, 0.6f)
		);

		// cave box 3
		addNewBrushS(
			"white",
			plainTexture,
			glm::vec3(31.0f, 3.0f, 15.0f),
			45.0f,
			glm::vec3(1.0f, 0.8f, 0.0f),
			glm::vec3(0.6f, 0.6f, 0.6f)
		);

		// cave box 4
		addNewBrushS(
			"white",
			plainTexture,
			glm::vec3(31.0f, 3.0f, 6.0f),
			45.0f,
			glm::vec3(0.0f, 1.3f, 1.0f),
			glm::vec3(0.6f, 0.6f, 0.6f)
		);

		// cave box 5
		addNewBrushS(
			"white",
			plainTexture,
			glm::vec3(26.0f, 3.0f, 7.0f),
			45.0f,
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(0.6f, 0.6f, 0.6f)
		);

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

		// floor 5 (alley)
		addNewBrushS(
			"green",
			grassTexture,
			glm::vec3(18.0f, 0.0f, 10.0f),
			0.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(6.0f, 1.0f, 5.0f));

		// ceiling 5 (alley)
		addNewBrushS(
			"green",
			rustTexture,
			glm::vec3(18.0f, 6.0f, 10.0f),
			0.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(6.0f, 1.0f, 7.0f));

		// floor 6 (cave)
		addNewBrushS(
			"green",
			dirtTexture,
			glm::vec3(31.0f, 0.0f, 10.0f),
			0.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(20.0f, 1.0f, 20.0f));

		// ceiling 6 (cave)
		addNewBrushS(
			"green",
			rustTexture,
			glm::vec3(31.0f, 6.0f, 10.0f),
			0.0f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(20.0f, 1.0f, 20.0f));

		// left front wall cave
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(21.5f, 3.0f, 3.75f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 5.0f, 7.5f)
		);

		// right front wall cave
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(21.5f, 3.0f, 16.25f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 5.0f, 7.5f)
		);

		// back wall cave
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(40.5f, 3.0f, 10.0f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 5.0f, 20.0f)
		);

		// left wall cave
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(31.0f, 3.0f, 0.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(18.0f, 5.0f, 1.0f)
		);

		// right wall cave
		addNewBrushS(
			"red",
			brickTexture,
			glm::vec3(31.0f, 3.0f, 19.5f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(18.0f, 5.0f, 1.0f)
		);


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
			glm::vec3(15.5f, 3.0f, 1.25f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(12.5f, 5.0f, 1.0f)
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
			glm::vec3(15.5f, 3.0f, 13.75f),
			90.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(2.5f, 5.0f, 1.0f)
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

		// Left Wall alley
		addNewBrushS(
			"blue",
			brickTexture,
			glm::vec3(18.5f, 3.0f, 7.0f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(5.0f, 5.0f, 1.0f)
		);

		// right Wall alley
		addNewBrushS(
			"blue",
			brickTexture,
			glm::vec3(18.5f, 3.0f, 13.0f),
			0.0f,
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(5.0f, 5.0f, 1.0f)
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
		plant.render();
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
			//omniShadowMapPass(&singlePointLight);
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
		//shaders[0]->setPointLights(pointLights, 3);
		//shaders[0]->setSpotLights(spotLights, 4);

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
		for (auto &prism : prisms2) {
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
};

