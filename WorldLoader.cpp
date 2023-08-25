#include "WorldLoader.hpp"

std::vector<RectangularPrism> WorldLoader::m_prisms 
	= std::vector<RectangularPrism>();

std::vector<Texture *> WorldLoader::m_textures = std::vector<Texture *>();

std::vector<Material> WorldLoader::m_materials = std::vector<Material>();

std::vector<Shader *> WorldLoader::m_shaders = std::vector<Shader *>();

std::vector<MeshGroup> WorldLoader::m_meshGroups = std::vector<MeshGroup>();

Skybox WorldLoader::m_skybox;

DirectionalLight WorldLoader::m_directionalLight;
PointLight WorldLoader::m_pointLights[MAX_POINT_LIGHTS] = {};
SpotLight WorldLoader::m_spotLights[MAX_SPOT_LIGHTS] = {};



void WorldLoader::load() {
	WorldLoader::loadShaders();
	WorldLoader::loadTextures();
	WorldLoader::loadMaterials();
	WorldLoader::loadBrushes();
	WorldLoader::loadSkybox();
	WorldLoader::loadDirectionalLights();
	WorldLoader::loadPointLights();
	WorldLoader::loadSpotLights();
	WorldLoader::loadMeshGroups();
}

void WorldLoader::loadSkybox() {
	std::string skyboxFaces[6] = {
		"resources/skyboxes/ame_nebula/purplenebula_rt.tga",
		"resources/skyboxes/ame_nebula/purplenebula_lf.tga",
		"resources/skyboxes/ame_nebula/purplenebula_up.tga",
		"resources/skyboxes/ame_nebula/purplenebula_dn.tga",
		"resources/skyboxes/ame_nebula/purplenebula_bk.tga",
		"resources/skyboxes/ame_nebula/purplenebula_ft.tga"
	};
	WorldLoader::m_skybox = Skybox(skyboxFaces);
}

void WorldLoader::loadDirectionalLights() {
	WorldLoader::m_directionalLight = DirectionalLight(
		glm::vec3(10.0f, 15.0f, 20.0f),
		glm::vec3(1.0f, 0.9f, 0.9f),
		0.001f,
		0.1f
	);
	WorldLoader::m_directionalLight.initShadowMap();
}

void WorldLoader::loadPointLights() {
	WorldLoader::m_pointLights[0] = PointLight( // cave light
		glm::vec3(28.5f, 2.0f, 10.0f),
		glm::vec3(1.0f, 0.5f, 1.0f)
	);
	WorldLoader::m_pointLights[0].setIntensity(0.4, 0.8);
	WorldLoader::m_pointLights[0].setFalloff(0.7f, 0.3f, 0.12f);

	WorldLoader::m_pointLights[1] = PointLight(
		glm::vec3(8.0f, 2.5f, -0.5f),
		glm::vec3(1.0f, 1.0f, 0.0f)
	);
	WorldLoader::m_pointLights[2] = PointLight(
		glm::vec3(10.0f, 6.0f, 22.5f),
		glm::vec3(1.0f, 0.5f, 0.0f)
	);
	WorldLoader::m_pointLights[2].setIntensity(0.4f, 0.8f);


	for (auto &pl : WorldLoader::m_pointLights) {
		pl.initOmniShadowMap();
	}
}

void WorldLoader::loadSpotLights() {
	SpotLight sp1 = SpotLight(glm::vec3(-1.0f, 12.0f, 0.0f));
	sp1.setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
	sp1.setEdge(20.0f);
	sp1.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	WorldLoader::m_spotLights[0] = sp1;

	SpotLight sp2 = SpotLight(glm::vec3(12.0f, 4.0f, 5.0f));
	sp2.setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
	sp2.setConstantFactor(0.02f);
	sp2.setLinearFactor(0.006f);
	sp2.setEdge(30.0f);
	sp2.setColor(glm::vec3(1.0f, 0.0f, 1.0f));
	WorldLoader::m_spotLights[1] = sp2;

	SpotLight sp3 = SpotLight(glm::vec3(10.0f, 3.0f, 14.5f));
	sp3.setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
	sp3.setEdge(65.0f);
	sp3.setColor(glm::vec3(0.0f, 1.0f, 1.0f));
	WorldLoader::m_spotLights[2] = sp3;

	// Flashlight
	SpotLight sp4 = SpotLight(glm::vec3(0.0f));
	sp4.setDirection(glm::vec3(0.0f));
	sp4.setEdge(10.0f); 
	sp4.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	sp4.setIntensity(0.2f, 0.8f);
	WorldLoader::m_spotLights[3] = sp4;

	for (auto &sl : WorldLoader::m_spotLights) {
		sl.initOmniShadowMap();
	}
}

void WorldLoader::loadTextures() {
	WorldLoader::m_textures.push_back(new Texture("resources/brick.png"));
	WorldLoader::m_textures[0]->loadTextureAlphaOption(true);

	WorldLoader::m_textures.push_back(new Texture("resources/dirt.png"));
	WorldLoader::m_textures[1]->loadTextureAlphaOption(true);

	WorldLoader::m_textures.push_back(new Texture("resources/grass.png"));
	WorldLoader::m_textures[2]->loadTextureAlphaOption(true);

	WorldLoader::m_textures.push_back(new Texture("resources/box.png"));
	WorldLoader::m_textures[3]->loadTextureAlphaOption(true);

	WorldLoader::m_textures.push_back(new Texture("resources/rusty.png"));
	WorldLoader::m_textures[4]->loadTextureAlphaOption(true);

	WorldLoader::m_textures.push_back(new Texture("resources/plain.png"));
	WorldLoader::m_textures[5]->loadTextureAlphaOption(true);
}

void WorldLoader::loadMaterials() {
	Material shinyMaterial = Material(1.0f, 32);
	WorldLoader::m_materials.push_back(shinyMaterial);

	Material dullMaterial = Material(1.0f, 64);
	WorldLoader::m_materials.push_back(dullMaterial);

	Material superShinyMaterial = Material(1.0f, 128);
	WorldLoader::m_materials.push_back(superShinyMaterial);
}

void WorldLoader::loadShaders() {
	std::string vShader = "vertexShader.vert";
	std::string fShader = "fragmentShader.frag";

	std::string vShader1 = "vDirectionalShadowMap.vert";
	std::string fShader1 = "fDirectionalShadowMap.frag";

	std::string vShader2 = "omniShadowMap.vert";
	std::string gShader2 = "omniShadowMap.geom";
	std::string fShader2 = "omniShadowMap.frag";

	// shaders for deferred rendering
	std::string dr_vshader = "DRgeom.vert";
	std::string dr_fshader = "DRgeom.frag";

	std::string dr_vshaderL = "DRlighting.vert";
	std::string dr_fshaderL = "DRlighting.frag";

	Shader *shader1 = new Shader();
	shader1->createFromFile(vShader, fShader);
	WorldLoader::m_shaders.push_back(shader1);

	Shader *shader2 = new Shader();
	shader2->createFromFile(vShader1, fShader1);
	WorldLoader::m_shaders.push_back(shader2);

	Shader *shader3 = new Shader();
	shader3->createFromFile(vShader2, gShader2, fShader2);
	WorldLoader::m_shaders.push_back(shader3);

	//Shader *shader4 = new Shader();
	//shader4->createFromFile(dr_vshader, dr_fshader);
	//WorldLoader::m_shaders.push_back(shader4);

	//Shader *shader5 = new Shader();
	//shader5->createFromFile(dr_vshaderL, dr_fshaderL);
	//WorldLoader::m_shaders.push_back(shader5);
}

void WorldLoader::loadMeshGroups() {
	MeshGroup cat = MeshGroup();
	cat.load("resources/12221_Cat_v1_l3.obj");
	WorldLoader::m_meshGroups.push_back(cat);
}


void WorldLoader::addNewBrushS(
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
	WorldLoader::m_prisms.push_back(rp);
}

void WorldLoader::loadBrushes() {
	Texture *brickTexture = m_textures[0];
	Texture *dirtTexture = m_textures[1];
	Texture *grassTexture = m_textures[2];
	Texture *boxTexture = m_textures[3];
	Texture *rustTexture = m_textures[4];
	Texture *plainTexture = m_textures[5];

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
