#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

class Model
{

public:
	Model(char *path);
	void Draw(Shader* shader);

	void Release();
private:
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(std::string file, std::string directory);

public:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;

private:
	/* Model Data */
	std::string directory;
};

#endif

