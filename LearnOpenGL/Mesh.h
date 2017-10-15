#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{

public:
/* Functions */
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	~Mesh(); 
	void Draw(Shader* shader);

private:
	void setupMesh();

/* Mesh Data */
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

private:
	unsigned int VAO, VBO, EBO;


};

#endif

