#include "Renderer.h"
#include "ImageLoader.h"
#include <vector>
struct MyMesh
{
	GLuint VAO;
	unsigned int numElements;

};
GLuint tex;
bool CreateTerrain(int numCellsX, int numCellsZ, float sizeX, float sizeZ, const std::string&
	textureFilename);
std::vector<MyMesh>MyMeshVector;
std::vector<glm::vec3> terrainpositions;
std::vector<glm::vec3> terrainnormals;
std::vector<glm::vec3> terrainuv;
std::vector<GLuint> terrainelements;
GLuint verticesVBO;
GLuint elementsEBO;
GLuint normalsNBO;
GLuint texturesTBO;
// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	glDeleteProgram(m_program);	
	glDeleteBuffers(1, &m_VAO);
}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram()
{
	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.glsl") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.glsl") };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_program, vertex_shader);

	// The attibute 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	return !Helpers::CheckForGLError();
}



// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;
//	if (!heightmap.LoadFromFile("Data\\Sky\\Mars\\Mar_R"))
//{
//	std::cerr << "not in file", "can't open" << std::endl;
//}




CreateTerrain(2, 2, 10.0f, 10.0f, "Data\\Models\\grass11.bmp");
glGenVertexArrays(1, &m_VAO);
glBindVertexArray(m_VAO);
glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
glEnableVertexAttribArray(0);
glVertexAttribPointer(
	0,                  // attribute 0
	3,                  // size in bytes of each item in the stream
	GL_FLOAT,           // type of the item
	GL_FALSE,           // normalized or not (advanced)
	0,                  // stride (advanced)
	(void*)0            // array buffer offset (advanced)
);

glBindBuffer(GL_ARRAY_BUFFER, normalsNBO);
glEnableVertexAttribArray(1);
glVertexAttribPointer(
	1,                  // attribute 1
	3,                  // size in bytes of each item in the stream
	GL_FLOAT,           // type of the item
	GL_FALSE,           // normalized or not (advanced)
	0,                  // stride (advanced)
	(void*)0            // array buffer offset (advanced)
);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
glBindBuffer(GL_ARRAY_BUFFER, texturesTBO);
glEnableVertexAttribArray(2);
glVertexAttribPointer(
	2,                  // attribute 0
	2,                  // size in bytes of each item in the stream 2vectors not 3 so it works!!
	GL_FLOAT,           // type of the item
	GL_FALSE,           // normalized or not (advanced)
	0,                  // stride (advanced)
	(void*)0            // array buffer offset (advanced)
);
//add to vector


//return true;


	//Helpers::ImageLoader texture;
	//if (!texture.Load("Data\\Models\\Jeep\\jeep_army.jpg"))
	//{
	//	std::cerr << "Not in file ! , Sorry!" << std::endl;
	//}
	//glGenTextures(1, &tex);
	//glBindTexture(GL_TEXTURE_2D, tex);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	//	GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.Width(), texture.Height(), 0,
	//	GL_RGBA, GL_UNSIGNED_BYTE, texture.GetData());
	//glGenerateMipmap(GL_TEXTURE_2D);
	//Helpers::ModelLoader textureloader;
	//
	
	


	// Good idea to check for an error now:	
	Helpers::CheckForGLError();

	// Clear VAO binding
	glBindVertexArray(0);

	return true;
}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{		
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Uncomment to render in wireframe (can be useful when debugging)
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// TODO: Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 2000.0f);

	// TODO: Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 combined_xform = projection_xform * view_xform;
	glm::mat4 model_xform(1);// = glm::translate(glm::mat4(1.0), glm::vec3(1, 1, 1));

	glUseProgram(m_program);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(m_program, "sampler_tex"),0);
	// TODO: Send the combined matrix to the shader in a uniform
	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));
	GLuint model_xform_id= glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));
	// TODO: render each mesh. Send the correct model matrix to the shader in a uniform

	glBindVertexArray(MyMeshVector[0].VAO);
	glDrawElements(GL_TRIANGLES,MyMeshVector[0].numElements, GL_UNSIGNED_INT, (void*)0);

	// Always a good idea, when debugging at least, to check for GL errors
	Helpers::CheckForGLError();
}

bool CreateTerrain(int numCellsX, int numCellsZ, float sizex, float sizez, const std::string& textureFilename)
{
	Helpers::ModelLoader loader;
	if (!loader.LoadFromFile("Data\\Models\\grass11.bmp"))
	{
		std::cerr << "Not in file, cannot open" << std::endl;
	}
	bool toggleDiamondpattern = true;
	int numCells = 4;
	 numCellsX = 2;
	 numCellsZ = 2;
	int numVertsX{ numCellsX + 1 };
	int numVertsZ{ numCellsZ + 1 };
	int numVerts{ numVertsX * numVertsZ };
    sizex = 10.0f;
	sizez = 10.0f;
	

	for (int cellz = 0; cellz < numVertsZ; cellz++)
	{
		for (int cellx = 0; cellx < numVertsX; cellx++)
		{
			terrainpositions.push_back({ cellx * sizex, 0, -cellz * sizez });
		}

	}

	for (int cellz = 0; cellz < numVertsZ; cellz++)
	{
		for (int cellx = 0; cellx < numVertsX; cellx++)
		{
			int startVertIndex = cellz * numVertsX + cellx;

			if (toggleDiamondpattern)
			{
				//first triangle
				terrainelements.push_back(startVertIndex);
				terrainelements.push_back(startVertIndex + 1);
				terrainelements.push_back(startVertIndex + numVertsX);
				//second triangle
				terrainelements.push_back(startVertIndex + 1);
				terrainelements.push_back(startVertIndex + numVertsX + 1);
				terrainelements.push_back(startVertIndex + numVertsX);
			}
			else
			{
				//first triangle
				terrainelements.push_back(startVertIndex + numVertsX + 1);
				terrainelements.push_back(startVertIndex + numVertsX);
				terrainelements.push_back(startVertIndex);
				//second triangle
				terrainelements.push_back(startVertIndex);
				terrainelements.push_back(startVertIndex + 1);
				terrainelements.push_back(startVertIndex + numVertsX + 1);


			}

		}

	}
	for (const Helpers::Mesh& mesh : loader.GetMeshVector())
	{
		mesh.uvCoords;
		//data you can extract from the mesh
		//mesh.vertices; //glm::vec3 vertices
		//mesh.elements; //gives unsigned int elements
		//mesh.normals; //glm::vec3
		//create Vbo's
		
		glGenBuffers(1, &verticesVBO);
		glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
		glGenBuffers(1, &elementsEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * terrainelements.size(), terrainelements.data(), GL_STATIC_DRAW);
		
		glGenBuffers(1, &normalsNBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsNBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);
		glGenBuffers(1, &texturesTBO);
		glBindBuffer(GL_ARRAY_BUFFER, texturesTBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
		//create vao's
		
	}
}
