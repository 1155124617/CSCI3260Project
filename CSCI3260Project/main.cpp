//
//  main.cpp
//  CSCI3260Project
//
//  Created by walter on 22/11/2021.
// Student 1: DING Baizeng
// SID     1: 1155124617
// Student 2: DAI Zijie
// SID     2: 1155141656
//

/*#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"*/
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "Dependencies/stb_image/stb_image.h"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Texture.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>

using namespace std;
using namespace glm;

// screen setting
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

//-------------------------------Self-designed variables and structures:-----------------------------

struct Path{
	//Files for planet
	const char* planet = "./Resources/object/planet.obj";
	const char* planet_normal_texture = "./Resources/texture/earthNormal.bmp";
	const char* planet_image_texture = "./Resources/texture/earthTexture.bmp";

	//Files for craft
	const char* craft = "./Resources/object/craft.obj";
	const char* red_Texture = "./Resources/texture/red.bmp";
	const char* ring_texture = "./Resources/texture/ringTexture.bmp";

	//Files for rock
	const char* rock = "./Resources/object/rock.obj";
	const char* rock_texture = "./Resources/texture/rockTexture.bmp";
	const char* gold_texture = "./Resources/texture/gold.bmp";

	//Files for spacecraft
	const char* spacecraft = "./Resources/object/spacecraft.obj";
	const char* spacecraft_texutre = "./Resources/texture/spacecraftTexture.bmp";

	//Texures for skybox
	const char* skybox_back = "./Resources/texture/skybox textures/back.bmp";
	const char* skybox_front= "./Resources/texture/skybox textures/front.bmp";
	const char* skybox_left= "./Resources/texture/skybox textures/left.bmp";
	const char* skybox_right= "./Resources/texture/skybox textures/right.bmp";
	const char* skybox_top= "./Resources/texture/skybox textures/top.bmp";
	const char* skybox_bottom= "./Resources/texture/skybox textures/bottom.bmp";
};

struct SpaceCraftMovement {
    bool up_pressed = false;
    bool down_pressed = false;
    bool left_pressed = false;
    bool right_pressed = false;
    float speed = 0.1;
};


//Structure Variables:
Shader shader, skyboxShader;
Path path;
SpaceCraftMovement spmv;

// Control cursor movement
bool firstMouse = true;
float yaw   = -90.0f,
pitch =  0.0f,
lastX =  800.0f / 2.0,
lastY =  600.0 / 2.0;


//VAO & EBO values for the models:
GLuint planetVAO, planetEBO,
craftVAO, craftEBO,
rockVAO, rockEBO,
spacecraftVAO, spacecraftEBO,
skyboxVAO, skyboxEBO;

//Texture vriables:
Texture rockTexture, spacecraftTexture[2],craftTexture[2],planetTexture;
GLuint cubemapTexture;

GLuint programID;

//Parameters for spacecraft:
float spftPosX = 0.0f,
spftPosY = -0.1f,
spftPosZ = 10.0f,
camera_offset_Z = 0.85f,
camera_offset_Y = 0.3f;
bool goldCollected = false;

float last_time = 0.0f;
	
vec3 targetDirection = vec3(0.0f, 0.0f, -1.0f);

//Parameters for lighting:
float directionalIntensity = 0.4f;
//--Point Light:  distance: 50
float pointLightX=10.0f,
pointLightY=10.0f,
pointLightZ=10.0f,
pointLightConstant = 1.0f,
pointLightLinear = 0.045f,
pointLightQudratic = 0.0075f,
pointLightIntensity=1.0f;

//Parameters for crafts:
float rotate_speed = 0.5f;

//Scale factor: control the size of all object
float scaleFactor = 0.2f;

// Dimensions for model objects
vec3 spft_dim = vec3(737.25 * scaleFactor * 0.002f, 151.1 * scaleFactor * 0.002f, 459.5 * scaleFactor * 0.002f),
planet_dim = vec3(2.60687 * scaleFactor * 1.5f, 2.60687 * scaleFactor * 1.5f, 2.60697 * scaleFactor * 1.5f),
loft_dim=  vec3(6.2472 * scaleFactor * 0.3, 3.76656 * scaleFactor * 0.3, 6.2472 * scaleFactor * 0.3);

//Boolean values for crafts collision detection (Order from near to far corresponding to the planet
bool collision_near = false;
bool collision_middle = false;
bool collision_far = false;


//--------------------------------------------------End-------------------------------------------






// struct for storing the obj file
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};


Model Planet, Rock, Spacecraft,Craft;



Model loadOBJ(const char* objPath)
{
	// function to load the obj file
	// Note: this simple function cannot load all obj files.

	struct V {
		// struct for identify if a vertex has showed up
		unsigned int index_position, index_uv, index_normal;
		bool operator == (const V& v) const {
			return index_position == v.index_position && index_uv == v.index_uv && index_normal == v.index_normal;
		}
		bool operator < (const V& v) const {
			return (index_position < v.index_position) ||
				(index_position == v.index_position && index_uv < v.index_uv) ||
				(index_position == v.index_position && index_uv == v.index_uv && index_normal < v.index_normal);
		}
	};

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::map<V, unsigned int> temp_vertices;

	Model model;
	unsigned int num_vertices = 0;

	std::cout << "\nLoading OBJ file " << objPath << "..." << std::endl;

	std::ifstream file;
	file.open(objPath);

	// Check for Error
	if (file.fail()) {
		std::cerr << "Impossible to open the file! Do you use the right path? See Tutorial 6 for details" << std::endl;
		exit(1);
	}
    float max_y = -100.0f;
    float min_y = 100.0f;
    float max_x = -100.0f;
    float min_x = 100.0f;
    float max_z = -100.0f;
    float min_z = 100.0f;

	while (!file.eof()) {
		// process the object file
		char lineHeader[128];
		file >> lineHeader;
		if (strcmp(lineHeader, "v") == 0) {
			// geometric vertices
			glm::vec3 position;
			file >> position.x >> position.y >> position.z;
            if (position.y > max_y) max_y = position.y;
            if (position.y < min_y) min_y = position.y;
            if (position.z > max_z) max_z = position.z;
            if (position.z < min_z) min_z = position.z;
            if (position.x > max_x) max_x = position.x;
            if (position.x < min_x) min_x = position.x;
			temp_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			// texture coordinates
			glm::vec2 uv;
			file >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			// vertex normals
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			// Face elements
			V vertices[3];
			for (int i = 0; i < 3; i++) {
				char ch;
				file >> vertices[i].index_position >> ch >> vertices[i].index_uv >> ch >> vertices[i].index_normal;
			}

			// Check if there are more than three vertices in one face.
			std::string redundency;
			std::getline(file, redundency);
			if (redundency.length() >= 5) {
				std::cerr << "There may exist some errors while load the obj file. Error content: [" << redundency << " ]" << std::endl;
				std::cerr << "Please note that we only support the faces drawing with triangles. There are more than three vertices in one face." << std::endl;
				std::cerr << "Your obj file can't be read properly by our simple parser :-( Try exporting with other options." << std::endl;
				exit(1);
			}

			for (int i = 0; i < 3; i++) {
				if (temp_vertices.find(vertices[i]) == temp_vertices.end()) {
					// the vertex never shows before
					Vertex vertex;
					vertex.position = temp_positions[vertices[i].index_position - 1];
					vertex.uv = temp_uvs[vertices[i].index_uv - 1];
					vertex.normal = temp_normals[vertices[i].index_normal - 1];

					model.vertices.push_back(vertex);
					model.indices.push_back(num_vertices);
					temp_vertices[vertices[i]] = num_vertices;
					num_vertices += 1;
				}
				else {
					// reuse the existing vertex
					unsigned int index = temp_vertices[vertices[i]];
					model.indices.push_back(index);
				}
			} // for
		} // else if
		else {
			// it's not a vertex, texture coordinate, normal or face
			char stupidBuffer[1024];
			file.getline(stupidBuffer, 1024);
		}
	}
    std::cout << "dimension: "<< "x: " << (max_x-min_x)/2 << " y: " << (max_y-min_y)/2 << " z: " << (max_z-min_z)/2 <<std::endl;
    std::cout << "should move " << -(max_y+min_y)/2  << std::endl;
	file.close();

	std::cout << "There are " << num_vertices << " vertices in the obj file.\n" << std::endl;
	return model;
}

GLuint loadCubemap(std::vector<const GLchar*> faces)
{
	int width, height, BPP;
	unsigned char* image;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		// tell stb_image.h to flip loaded texture's on the y-axis.
		stbi_set_flip_vertically_on_load(true);
		// load the texture data into "data"
		image = stbi_load(faces[i], &width, &height, &BPP, 0);
		if (image)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
				0, GL_RGB, GL_UNSIGNED_BYTE, image);
			stbi_image_free(image);
		}
		else
		{
			std::cout << "Failed to load cubemap texture" << std::endl;
			stbi_image_free(image);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

void get_OpenGL_info()
{
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}

GLuint collision_deteciton(vec3 object, vec3 ref, vec3 object_dim, vec3 ref_dim) {
    float object_ref_distance = glm::distance(object, ref);
    float collision_distance = glm::distance(object_dim - ref_dim, vec3(0.0f,0.0f,0.0f));
    if (object_ref_distance > collision_distance) return 0;
    else return 1;
}

void sendDataToOpenGL()
{
    struct Path paths;
    GLuint VBO;
    // load skybox
    GLfloat skyboxVertices[] =
    {
        // Positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    // CubeMap texture loading
    std::vector<const GLchar*> faces;
    faces.push_back(paths.skybox_right);
    faces.push_back(paths.skybox_left);
    faces.push_back(paths.skybox_top);
    faces.push_back(paths.skybox_bottom);
    faces.push_back(paths.skybox_bottom);
    faces.push_back(paths.skybox_front);
    cubemapTexture = loadCubemap(faces);
    
	//Load Planet:
	Planet = loadOBJ(paths.planet);
	planetTexture.setupTexture(paths.planet_image_texture);
	glGenVertexArrays(1, &planetVAO);
	glBindVertexArray(planetVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Planet.vertices.size() * sizeof(Vertex), &Planet.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &planetEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planetEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Planet.indices.size() * sizeof(unsigned int), &Planet.indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Load Spacecraft:
	Spacecraft = loadOBJ(paths.spacecraft);
	spacecraftTexture[0].setupTexture(paths.spacecraft_texutre);
	spacecraftTexture[1].setupTexture(paths.gold_texture);
	glGenVertexArrays(1, &spacecraftVAO);
	glBindVertexArray(spacecraftVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Spacecraft.vertices.size() * sizeof(Vertex), &Spacecraft.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &spacecraftEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spacecraftEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Spacecraft.indices.size() * sizeof(unsigned int), &Spacecraft.indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//Load Rock:
	Rock = loadOBJ(paths.rock);
	rockTexture.setupTexture(paths.rock_texture);
	glGenVertexArrays(1, &rockVAO);
	glBindVertexArray(rockVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Rock.vertices.size() * sizeof(Vertex), &Rock.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &rockEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rockEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Rock.indices.size() * sizeof(unsigned int), &Rock.indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Load Craft:
	Craft = loadOBJ(paths.craft);
	craftTexture[0].setupTexture(paths.ring_texture);
	craftTexture[1].setupTexture(paths.red_Texture);
	glGenVertexArrays(1, &craftVAO);
	glBindVertexArray(craftVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Craft.vertices.size() * sizeof(Vertex), &Craft.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &craftEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, craftEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Craft.indices.size() * sizeof(unsigned int), &Craft.indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initializedGL(void) //run only once
{
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW not OK." << std::endl;
	}

	get_OpenGL_info();
	sendDataToOpenGL();

	shader.setupShader("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
    skyboxShader.setupShader("VertexShaderCodeSkyBox.glsl", "FragmentShaderCodeSkyBox.glsl");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void paintGL(void)  //always run
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f); //specify the background color, this is just an example
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Matrices for view transformation:
    mat4 modelMatrix = mat4(1.0f),
        viewMatrix = mat4(1.0f),
        projectionMatrix = mat4(1.0f);
    
    if (spmv.up_pressed) {
        spftPosX += spmv.speed * targetDirection.x;
        spftPosY += spmv.speed * targetDirection.y;
        spftPosZ += spmv.speed * targetDirection.z;
    }
    if (spmv.down_pressed) {
        spftPosX -= spmv.speed * targetDirection.x;
        spftPosY -= spmv.speed * targetDirection.y;
        spftPosZ -= spmv.speed * targetDirection.z;
    }
    if (spmv.left_pressed) {
        vec3 rightvec = glm::normalize(glm::cross(targetDirection, vec3(0.0f,1.0f,0.0f)));
        spftPosX -= spmv.speed * rightvec.x;
        spftPosY -= spmv.speed * rightvec.y;
        spftPosZ -= spmv.speed * rightvec.z;
    }
    if (spmv.right_pressed) {
        vec3 rightvec = glm::normalize(glm::cross(targetDirection, vec3(0.0f,1.0f,0.0f)));
        spftPosX += spmv.speed * rightvec.x;
        spftPosY += spmv.speed * rightvec.y;
        spftPosZ += spmv.speed * rightvec.z;
    }
    
    
	vec3 cameraPosition(spftPosX, spftPosY+camera_offset_Y, spftPosZ+camera_offset_Z);

    // First draw sky box
    glDepthMask(GL_FALSE);
    skyboxShader.use();
    //Initialization of camera view
    projectionMatrix = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
    viewMatrix = lookAt(cameraPosition,
        cameraPosition + targetDirection,
        vec3(0.0f, 1.0f, 0.0f));
    viewMatrix = glm::mat4(glm::mat3(viewMatrix));
    skyboxShader.setMat4("view", viewMatrix);
    skyboxShader.setMat4("projection", projectionMatrix);
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    skyboxShader.setInt("skybox", 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    
    
    
    // Draw other elements
    shader.use();

    viewMatrix = lookAt(cameraPosition,
        cameraPosition + targetDirection,
        vec3(0.0f, 1.0f, 0.0f));
	shader.setMat4("view", viewMatrix);
	shader.setMat4("projection", projectionMatrix);
	shader.setVec3("eyePositionWorld", cameraPosition);

	//Initialization of Lighting:
	
	//Directional Light:
	shader.setFloat("dirLight.intensity", directionalIntensity);
	shader.setVec3("dirLight.direction", vec3(-1.0f, -1.0f, -1.0f));
	shader.setVec3("dirLight.ambient", vec3(0.2f, 0.2f, 0.2f));
	shader.setVec3("dirLight.diffuse", vec3(0.4f, 0.4f, 0.4f));
	shader.setVec3("dirLight.specular", vec3(0.5f, 0.5f, 0.5f));

	//Point Light:  Distance:50
	shader.setVec3("pointLight.position", vec3(pointLightX, pointLightY, pointLightZ));
	shader.setVec3("pointLight.ambient", 1.0f * vec3(0.237, 0.122f, 0.016f));
	shader.setVec3("pointLight.diffuse", 2.0f * vec3(0.237f, 0.122f, 0.016f));
	shader.setVec3("pointLight.specular", 5.0f * vec3(0.237f, 0.122f, 0.016f));
	shader.setFloat("pointLight.constant", pointLightConstant);
	shader.setFloat("pointLight.linear", pointLightLinear);
	shader.setFloat("pointLight.quadratic", pointLightQudratic);
	shader.setFloat("pointLight.intensity",pointLightIntensity);

	//Send camera information to the shader:
	shader.setVec3("eyePositionWorld", cameraPosition);

    //Draw Spacecraft:
    modelMatrix = mat4(1.0f);
    modelMatrix = translate(modelMatrix, cameraPosition);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-yaw-90),
        glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(pitch),
        glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = translate(modelMatrix, vec3(0.0f, -camera_offset_Y, -camera_offset_Z));
    modelMatrix = rotate(modelMatrix, (float)radians(180.0), vec3(0.0, 1.0, 0.0));
    modelMatrix = scale(modelMatrix, vec3(scaleFactor * 0.002, scaleFactor * 0.002, scaleFactor * 0.002));
    shader.setMat4("model", modelMatrix);
    spacecraftTexture[goldCollected].bind(0);
    shader.setInt("texureSampler0", 0);
    glBindVertexArray(spacecraftVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spacecraftEBO);
    glDrawElements(GL_TRIANGLES, Spacecraft.indices.size(), GL_UNSIGNED_INT, 0);
    // Regard the central point as the object position
    glm::vec4 spft_vec = modelMatrix * glm::vec4(0.0f,0.0f,0.0f,1.0f);
    
	//Draw Planet:
	modelMatrix = mat4(1.0f);
	modelMatrix = scale(modelMatrix, vec3(scaleFactor * 1.5f, scaleFactor * 1.5f, scaleFactor * 1.5f));
	modelMatrix = rotate(modelMatrix, (float)(glfwGetTime()* 0.1f), vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = rotate(modelMatrix, (float)radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = translate(modelMatrix, vec3(0.0f, -1.04894f, 0.0f));
	shader.setMat4("model", modelMatrix);
	planetTexture.bind(0);
	shader.setInt("texureSampler0", 0);
	glBindVertexArray(planetVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,planetEBO);
	glDrawElements(GL_TRIANGLES, Planet.indices.size(), GL_UNSIGNED_INT, 0);
    // Regard the central point as the object position
    glm::vec4 planet_vec = modelMatrix * glm::vec4(0.0f,0.0f,0.0f,1.0f);
    if (collision_deteciton(vec3(spft_vec), vec3(planet_vec), spft_dim, planet_dim) == 1)
        std::cout << "Collision detected" << std::endl;
        

	//Draw Local Crafts:
	modelMatrix = mat4(1.0f);
	modelMatrix = rotate(modelMatrix, rotate_speed*(float)glfwGetTime(), vec3(0.0, 1.0, 0.0));
	modelMatrix = translate(modelMatrix, vec3(0.0f, -0.1f, 5.0f));
	modelMatrix = scale(modelMatrix, vec3(scaleFactor * 0.3, scaleFactor * 0.3, scaleFactor * 0.3));
	shader.setMat4("model", modelMatrix);
	glBindVertexArray(craftVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, craftEBO);
    // Regard the central point as the object position
    glm::vec4 loft_vec = modelMatrix * glm::vec4(0.0f,0.0f,0.0f,1.0f);
    if (collision_deteciton(vec3(spft_vec), vec3(loft_vec), spft_dim, loft_dim) == 1) {
        collision_near = true;
        last_time = (float)glfwGetTime();
    }
    else
        if ((float)glfwGetTime() - last_time > 2)
            collision_near = false;
    craftTexture[collision_near].bind(0);
    shader.setInt("texureSampler0", 0);
    glDrawElements(GL_TRIANGLES, Craft.indices.size(), GL_UNSIGNED_INT, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Sets the mouse-button callback for the current window.	
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Sets the cursor position callback for the current window
    // This function is referred to learnopenGL website. (DING Baizeng)
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.10;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    targetDirection = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Sets the scoll callback for the current window.
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Sets the Keyboard callback for the current window.
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        spmv.up_pressed = true;
    }
    if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
        spmv.up_pressed = false;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        spmv.down_pressed = true;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
        spmv.down_pressed = false;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        spmv.left_pressed = true;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
        spmv.left_pressed = false;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        spmv.right_pressed = true;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
        spmv.right_pressed = false;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	glfwSetWindowShouldClose(window, true);
}


int main(int argc, char* argv[])
{
	GLFWwindow* window;

	/* Initialize the glfw */
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	/* glfw: configure; necessary for MAC */
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/*register callback functions*/
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);                                                                  //    
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	initializedGL();

	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		paintGL();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

