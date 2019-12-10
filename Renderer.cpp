#include "Renderer.h"
#include "ImageLoader.h"
#include <vector>
struct MyMesh
{
	GLuint VAO;
	unsigned int numElements;
	GLuint textureId;

};

std::vector<MyMesh>MyMeshVector;
// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	glDeleteProgram(m_program);	
	glDeleteBuffers(1, &VAO);
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


bool Renderer::CreateTerrain()
{
	Helpers::ModelLoader loader;
	if (!loader.LoadFromFile("Data\\Models\\grass11.bmp"))
	{
		std::cerr << "Not in file, cannot open" << std::endl;
	}

	float size{ 1000 };

	bool toggle{ true };
	int numCellsXZ{ 2 };

	float cellSize = size / numCellsXZ;

	int numVertsX = numCellsXZ + 1;


	int numVertsZ = numCellsXZ + 1;

	glm::vec3 start(-size / 2, 0, size / 2);
	std::vector<glm::vec3>vertices;
	std::vector<glm::vec2>uvCoords;
	float tiles{ 10.0f };
	for (int z = 0; z < numVertsZ; z++)
	{
		for (int x = 0; x < numVertsX; x++)
		{
			glm::vec3 pos{ start };
			pos.x += x * cellSize;
			pos.z += z * cellSize;
			pos.y = rand() % 200;
			vertices.push_back(pos);
			float u = (float)x / (numVertsX - 1);
			float v = (float)z / (numVertsZ - 1);
			u *= tiles;
			v *= tiles;
			uvCoords.push_back(glm::vec2(u, v));
		}
	}

	std::vector<GLuint>elements;
	for (int cellZ = 0; cellZ < numCellsXZ; cellZ++)
	{
		for (int cellX = 0; cellX < numCellsXZ; cellX++)
		{
			int startVertIndex = cellZ * numVertsX * cellX;
			//first triangle
			if (toggle)
			{
				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertsX + 1);

				//second triangle
				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + numVertsX + 1);
				elements.push_back(startVertIndex + numVertsX);
			}
			else
			{
				//first triangle
				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertsX);
				//second triangle
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertsX + 1);
				elements.push_back(startVertIndex + numVertsX);
			}
			toggle = !toggle;
		}
		toggle = !toggle;
	}
	Model newModel;
	newModel.name = name;

	GLuint positionsVBO;
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLuint coordsVBO;
	glGenBuffers(1, &coordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, coordsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * uvCoords.size(), uvCoords.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLuint elementsEBO;
	glGenBuffers(1, &elementsEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // size in bytes of each item in the stream
		GL_FLOAT,           // type of the item
		GL_FALSE,           // normalized or not (advanced)
		0,                  // stride (advanced)
		(void*)0            // array buffer offset (advanced)
	);
	glBindBuffer(GL_ARRAY_BUFFER, coordsVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute 0
		2,                  // size in bytes of each item in the stream
		GL_FLOAT,           // type of the item
		GL_FALSE,           // normalized or not (advanced)
		0,                  // stride (advanced)
		(void*)0            // array buffer offset (advanced)
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBindVertexArray(0);
	Helpers::CheckForGLError();

	MyMesh newMesh;
	newMesh.VAO = VAO;
	newMesh.numElements = (GLuint)elements.size();
	Helpers::ImageLoader image;
	if (image.Load("Data\\Textures\\grass11.bmp"))
	{
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width(), image.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);
		newMesh.textureId = tex;
	}
	else
	{
		std::cout << "Texture load eror" << std::endl;
	}

	newModel.mesh.push_back(newMesh);
	Helpers::CheckForGLError;
	models.push_back(newModel);
	return true;
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;
	if (!CreateTerrain())
		return false;
//	if (!heightmap.LoadFromFile("Data\\Sky\\Mars\\Mar_R"))
//{
//	std::cerr << "not in file", "can't open" << std::endl;
//}

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
	
	
	// TODO: Send the combined matrix to the shader in a uniform
	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));
	
	// TODO: render each mesh. Send the correct model matrix to the shader in a uniform
	for (const auto& currentmesh : mesh)
	{ 
		glm::mat4 model_xform = glm::mat4(1);
		GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
		glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));
		if (mesh.textureId)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.textureId);
			glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glBindVertexArray();
		glDrawElements(GL_TRIANGLES, mesh.numElements, GL_UNSIGNED_INT, (void*)0);

		// Always a good idea, when debugging at least, to check for GL errors
		Helpers::CheckForGLError();
	}
}


