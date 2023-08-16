#pragma once

#include <vector>

#include "RectangularPrism.hpp"
#include "SpotLight.hpp"
#include "PointLight.hpp"
#include "DirectionalLight.hpp"
#include "Skybox.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "MeshGroup.hpp"

class WorldLoader {
public:

	static std::vector<Texture *> m_textures;
	static std::vector<Material> m_materials;
	static std::vector<Shader *> m_shaders;
	static std::vector<RectangularPrism> m_prisms;
	static std::vector<MeshGroup> m_meshGroups;
	
	static Skybox m_skybox;

	static DirectionalLight m_directionalLight;
	static PointLight m_pointLights[MAX_POINT_LIGHTS];
	static SpotLight m_spotLights[MAX_SPOT_LIGHTS];

	static void load();

	static void loadSkybox();

	static void loadDirectionalLights();

	static void loadPointLights();

	static void loadSpotLights();

	static void loadTextures();
	
	static void loadMaterials();

	static void loadShaders();

	static void loadMeshGroups();

	static void addNewBrushS(
		std::string color,
		Texture *texture,
		glm::vec3 translation,
		float rotationAngle,
		glm::vec3 rotVector,
		glm::vec3 scale
	);

	static void loadBrushes();
private:

};