#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"


struct ModelMesh
{
	// Vertex Array Object to wrap all render settings
	GLuint VAO{ 0 };

	// Number of elments to use when rendering
	GLuint numElements{ 0 };

	GLuint textureId{ 0 };
};
struct Model
{
	std::string name;
	std::vector<ModelMesh>mesh;
};
class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };
	
	bool CreateProgram();
public:
	Renderer()=default;
	bool CreateTerrain();
	~Renderer();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);
};

