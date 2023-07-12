#include "MeshGroup.hpp"

MeshGroup::MeshGroup() {
}

MeshGroup::~MeshGroup() {
}

void MeshGroup::load(const std::string &filename) {

	// imports our scene
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(
		filename,
		aiProcess_Triangulate
		| aiProcess_FlipUVs
		| aiProcess_GenSmoothNormals
		| aiProcess_JoinIdenticalVertices
	);

	if (!scene) {
		std::cout << "meshgroup failed to load " << filename
			<< importer.GetErrorString() << std::endl;
		return;
	}

	loadNode(scene->mRootNode, scene);
	loadMaterials(scene);
}

void MeshGroup::render() {
	for (int i = 0; i < meshes.size(); ++i) {
		unsigned int materialIndex = meshesToTextures[i];
	
		// check if material exists in the textures
		// // and if its not null
		// check if we can even safely check 
		// the program evaluates the expression on the left first,
		// before evaluating the expression on the right
		// so if it fails, we won't even make it to the one on the right,
		// and thus avoid a segmentation fault
		if (materialIndex < textures.size() && textures[materialIndex]) {
			textures[materialIndex]->useTexture();
		}

		meshes[i]->renderMesh();
	}
}

void MeshGroup::clearData() {
	for (int i = 0; i < meshes.size(); ++i) {
		if (meshes[i]) {
			delete meshes[i];
			meshes[i] = nullptr;
		}
	}

	for (int i = 0; i < textures.size(); ++i) {
		if (textures[i]) {
			delete textures[i];
			textures[i] = nullptr;
		}
	}
}

void MeshGroup::loadNode(aiNode *node, const aiScene *scene) {
	for (int i = 0; i < node->mNumMeshes; ++i) {
		loadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}

	for (int i = 0; i < node->mNumChildren; ++i) {
		loadNode(node->mChildren[i], scene);
	}
}

void MeshGroup::loadMesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i < mesh->mNumVertices; ++i) {

		// Insert the positional data for the vertex
		vertices.insert(vertices.end(), {
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z,
		});

		//std::cout << "loaded\n";

		if (mesh->mColors[0]) {
			// Insert the color data for each vertex
			vertices.insert(vertices.end(), {
				mesh->mColors[i]->r,
				mesh->mColors[i]->g,
				mesh->mColors[i]->b
				});
		} else {
			vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
		}

		// Checking if there are textures, by checking
		// if the first texture even exists
		if (mesh->mTextureCoords[0]) {
			vertices.insert(vertices.end(), {
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			});

		// No texture
		} else {
			vertices.insert(vertices.end(), {0.0f, 0.0f});
		}

		// insert the normals
		// Because of the difference in convention with how we handle
		// our normals vs how assimp handles normals, we have to 
		// make these normals reversed
		vertices.insert(vertices.end(), {
			-mesh->mNormals[i].x,
			-mesh->mNormals[i].y,
			-mesh->mNormals[i].z
		});
	}

	// go through each face and add our inddices
	for (int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// create our mesh and add it to our mesh list
	Mesh *_mesh = new Mesh();
	_mesh->createMesh(&vertices[0], &indices[0],
		vertices.size(), indices.size());

	meshes.push_back(_mesh);

	// say which texture this particular mesh is pointing to
	meshesToTextures.push_back(mesh->mMaterialIndex);
}

void MeshGroup::loadMaterials(const aiScene *scene) {
	// auto-sizes the texture list so we don't need to push back
	textures.resize(scene->mNumMaterials);

	for (int i = 0; i < scene->mNumMaterials; ++i) {

		// if we have successfully found the material we want
		aiMaterial *material = scene->mMaterials[i];
		// we will put that same material in the texture list
		// that is was in the material list
		textures[i] = nullptr;

		// check for the existence of diffuse/texture map
		if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
			// if we have at least one diffuse texture in our material
			// get the path of the texture out of the material
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
				std::string pathData = std::string(path.data);
				int index = std::string(path.data).rfind("/");
				// Create a new substring at the last token in 
				// the file path (the file name)
				std::string filename = std::string(path.data).substr(index + 1);
			
				std::string texPath = std::string("/") + filename;

				//textures[i] = new Texture(texPath.c_str());
				textures[i] = new Texture(pathData);

				// for now no alpha channels
				if (!textures[i]->loadTextureAlphaOption(false)) {
					std::cout << "failed to load texture at : "
						<< texPath << std::endl;
					delete textures[i];
					textures[i] = nullptr;
				}
			}
		}
	
		if (!textures[i]) {
			textures[i] = new Texture("resources/plain.png");
			textures[i]->loadTextureAlphaOption(true);
		}
	}

}
