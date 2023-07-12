#pragma once

#include <vector>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h> // holds scene data that importer places values in
#include <assimp/postprocess.h>

#include "Mesh.hpp"
#include "Texture.hpp"

class MeshGroup {
public:
	MeshGroup();
	~MeshGroup();

	void load(const std::string &filename);
	void render();
	void clearData();

private:
	void loadNode(aiNode *node, const aiScene *scene);
	void loadMesh(aiMesh *mesh, const aiScene *scene);
	void loadMaterials(const aiScene *scene);

	std::vector<Mesh *> meshes;
	std::vector<Texture *> textures;
	std::vector<unsigned int> meshesToTextures;
};