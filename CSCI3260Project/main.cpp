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
#include "Dependencies/glm/gtc/matrix_transform.hpp"
#include "Dependencies/stb_image/stb_image.h"*/

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
#include <random>

using namespace std;
using namespace glm;

// screen setting
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
const int NUM_BLOCK = 400;
const int NUM_GOLD = 10;
//-------------------------------Self-designed variables and structures:-----------------------------

struct Path {
	//Files for planet
	const char* planet = "./Resources/object/planet.obj";
	const char* planet_normal_texture = "./Resources/texture/earthNormal.bmp";
	const char* planet_image_texture = "./Resources/texture/earthTexture.bmp";

	//Files for craft
	const char* craft = "./Resources/object/craft.obj";
	const char* red_Texture = "./Resources/texture/red.png";
	const char* ring_texture = "./Resources/texture/ringTexture.bmp";

	//Files for rock
	const char* rock = "./Resources/object/rock.obj";
	const char* rock_texture = "./Resources/texture/rockTexture.bmp";
	const char* gold_texture = "./Resources/texture/gold.png";

	//Files for spacecraft
	const char* spacecraft = "./Resources/object/spacecraft.obj";
	const char* spacecraft_texutre = "./Resources/texture/spacecraftTexture.bmp";

	//Files for skybox
	const char* skybox_back = "./Resources/texture/skybox textures/back.bmp";
	const char* skybox_front = "./Resources/texture/skybox textures/front.bmp";
	const char* skybox_left = "./Resources/texture/skybox textures/left.bmp";
	const char* skybox_right = "./Resources/texture/skybox textures/right.bmp";
	const char* skybox_top = "./Resources/texture/skybox textures/top.bmp";
	const char* skybox_bottom = "./Resources/texture/skybox textures/bottom.bmp";

	//Files for rocket
	const char* rocket = "./Resources/object/rocket.obj";
	const char* rocket_texture = "./Resources/texture/rocket.png";
	
	////Files for coin
	const char* coin = "./Resources/object/coin.obj";
	const char* coin_texture = "./Resources/texture/Coin.png";
};

struct SpaceCraftMovement {
	bool up_pressed = false;
	bool down_pressed = false;
	bool left_pressed = false;
	bool right_pressed = false;
	float speed = 0.1;
};
//Structure for rock ring
struct RockRing {

	float distance[NUM_BLOCK];
	float theta[NUM_BLOCK];
	float Y[NUM_BLOCK];
	float rotationSpeed = 0.2f;
	float selfRotationSpeed = 5.0f;
	float scale[NUM_BLOCK];
	int currentNumber = 1;
	bool isGold[NUM_BLOCK];
};
struct RockRing* rockRing = new struct RockRing;

//Structure for rockets
struct Rocket {
	vec3 initial_position = vec3(rand() % 10 - 5, 0.0f, rand() % 5 + 5);
	vec3 fire_direction;
	glm::mat4 fire_position_matrix;
	bool fire_first_determine = false;
	int state = 0;
	int loaded_position = 0;
	float timer = 0;
};
struct Rocket rockets[2];
int rockets_loaded = 0;
bool left_loaded = false;
bool right_loaded = false;
float rocket_speed = 25.0f;

//Structure Variables:
Shader shader, skyboxShader, planetShader;
Path path;
SpaceCraftMovement spmv;

// Control cursor movement
bool firstMouse = true;
float yaw = -90.0f,
pitch = 0.0f,
lastX = 800.0f / 2.0,
lastY = 600.0 / 2.0;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

//VAO & EBO values for the models:
GLuint planetVAO, planetEBO,
craftVAO, craftEBO,
rockVAO, rockEBO,
spacecraftVAO, spacecraftEBO,
skyboxVAO, skyboxEBO,
rocketVAO, rocketEBO,
coinVAO,coinEBO;

//Texture vriables:
Texture rockTexture[2], spacecraftTexture[2], craftTexture[2], planetTexture, rocketTexture,coinTexture,planetNormal;
GLuint cubemapTexture;

GLuint programID;

//Parameters for spacecraft:
float spftPosX = 0.0f,
spftPosY = -0.1f,
spftPosZ = 20.0f,
camera_offset_Z = 0.85f,
camera_offset_Y = 0.3f;
int goldCollected = 0;

float last_time = 0.0f;

vec3 targetDirection = vec3(0.0f, 0.0f, -1.0f);

//Parameters for lighting:
float directionalIntensity = 0.4f;
//--Point Light:  distance: 50
float pointLightX = 10.0f,
pointLightY = 10.0f,
pointLightZ = 10.0f,
pointLightConstant = 1.0f,
pointLightLinear = 0.045f,
pointLightQudratic = 0.0075f,
pointLightIntensity = 1.0f;

//Parameters for crafts:
float rotate_speed = 0.5f;

//Scale factor: control the size of all object
float scaleFactor = 0.2f;

// Dimensions for model objects
vec3 spft_dim = vec3(737.25 * scaleFactor * 0.002f, 151.1 * scaleFactor * 0.002f, 459.5 * scaleFactor * 0.002f),
planet_dim = vec3(2.60687 * scaleFactor * 1.5f, 2.60687 * scaleFactor * 1.5f, 2.60697 * scaleFactor * 1.5f),
loft_dim = vec3(6.2472 * scaleFactor * 0.3, 3.76656 * scaleFactor * 0.3, 6.2472 * scaleFactor * 0.3),
rock_dim = vec3(1.59172, 0.841796, 1.81414),
rocket_dim = vec3(5.92294 * scaleFactor * 0.5f, 1.55752 * scaleFactor * 0.5f, 1.55731 * scaleFactor * 0.5f);

//Boolean values for crafts collision detection (Order from near to far corresponding to the planet
bool collision_near = false;
bool collision_middle = false;
bool collision_far = false;
bool collision_horizontal_near = false;
bool collision_horizontal_middle = false;
bool collision_horizontal_far = false;
float holdTime[3];
float lastTime[3];
float horizontal_offset[3];
bool speedMode = false;
bool speedModeOn = false;
bool directionLightOn = true;
bool pointLightOn = true;
//--------------------------------------------------End-------------------------------------------






// struct for storing the obj file
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
    std::vector<Vertex> vertices_without_indices;
    /*std::vector<vec3> tangents;
    std::vector<vec3> bitangents;*/
};


Model Planet, Rock, Spacecraft, Craft, Rocket,Coin;

void decidePosition(struct RockRing* rockRing);
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

            glm::vec3 tangent;
            glm::vec3 bitangent;
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
                if (i % 3 == 0) {
                    glm::vec3 v0 = temp_positions[vertices[i+0].index_position - 1];
                    glm::vec3 v1 = temp_positions[vertices[i+1].index_position - 1];
                    glm::vec3 v2 = temp_positions[vertices[i+2].index_position - 1];
                    // Position delta
                    glm::vec3 deltaPos1 = v1 - v0;
                    glm::vec3 deltaPos2 = v2 - v0;
                    
                    glm::vec2 uv0 = temp_uvs[vertices[i+0].index_uv - 1];
                    glm::vec2 uv1 = temp_uvs[vertices[i+1].index_uv - 1];
                    glm::vec2 uv2 = temp_uvs[vertices[i+2].index_uv - 1];
                    // UV delta
                    glm::vec2 deltaUV1 = uv1 - uv0;
                    glm::vec2 deltaUV2 = uv2 - uv0;
                    
                    float factor = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
                    tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * factor;
                    bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * factor;
                    /*model.tangents.push_back(tangent);
                    model.bitangents.push_back(bitangent);*/
                }
                Vertex v;
                v.position = temp_positions[vertices[i].index_position - 1];
                v.uv = temp_uvs[vertices[i].index_uv - 1];
                v.normal = temp_normals[vertices[i].index_normal - 1];
                v.tangent = tangent;
                v.bitangent = bitangent;
                model.vertices_without_indices.push_back(v);
			} // for
		} // else if
		else {
			// it's not a vertex, texture coordinate, normal or face
			char stupidBuffer[1024];
			file.getline(stupidBuffer, 1024);
		}
	}
	std::cout << "dimension: " << "x: " << (max_x - min_x) / 2 << " y: " << (max_y - min_y) / 2 << " z: " << (max_z - min_z) / 2 << std::endl;
	std::cout << "should move " << -(max_y + min_y) / 2 << std::endl;
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

GLuint collision_detection(vec3 object, vec3 ref, vec3 object_dim, vec3 ref_dim) {
	float object_ref_distance = glm::distance(object, ref);
	float collision_distance = glm::distance(object_dim - ref_dim, vec3(0.0f, 0.0f, 0.0f));
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
    planetNormal.setupTexture(paths.planet_normal_texture);
	glGenVertexArrays(1, &planetVAO);
	glBindVertexArray(planetVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Planet.vertices_without_indices.size() * sizeof(Vertex), &Planet.vertices_without_indices[0], GL_STATIC_DRAW);
	//glGenBuffers(1, &planetEBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planetEBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, Planet.indices.size() * sizeof(unsigned int), &Planet.indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    


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
	rockTexture[0].setupTexture(paths.rock_texture);
	rockTexture[1].setupTexture(paths.gold_texture);
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

	//Load Rocket:
	Rocket = loadOBJ(paths.rocket);
	rocketTexture.setupTexture(paths.rocket_texture);
	glGenVertexArrays(1, &rocketVAO);
	glBindVertexArray(rocketVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Rocket.vertices.size() * sizeof(Vertex), &Rocket.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &rocketEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rocketEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Rocket.indices.size() * sizeof(unsigned int), &Rocket.indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	//Load Coin:
	Coin = loadOBJ(paths.coin);
	coinTexture.setupTexture(paths.coin_texture);
	glGenVertexArrays(1, &coinVAO);
	glBindVertexArray(coinVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Coin.vertices.size() * sizeof(Vertex), &Coin.vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &coinEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coinEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Coin.indices.size() * sizeof(unsigned int), &Coin.indices[0], GL_STATIC_DRAW);


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
	if (speedModeOn==true)
		spmv.speed = 0.4f;
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
		vec3 rightvec = glm::normalize(glm::cross(targetDirection, vec3(0.0f, 1.0f, 0.0f)));
		spftPosX -= spmv.speed * rightvec.x;
		spftPosY -= spmv.speed * rightvec.y;
		spftPosZ -= spmv.speed * rightvec.z;
	}
	if (spmv.right_pressed) {
		vec3 rightvec = glm::normalize(glm::cross(targetDirection, vec3(0.0f, 1.0f, 0.0f)));
		spftPosX += spmv.speed * rightvec.x;
		spftPosY += spmv.speed * rightvec.y;
		spftPosZ += spmv.speed * rightvec.z;
	}


	vec3 cameraPosition(spftPosX, spftPosY + camera_offset_Y, spftPosZ + camera_offset_Z);

	// First draw sky box
	glDepthMask(GL_FALSE);
	skyboxShader.use();
	//Initialization of camera view
	projectionMatrix = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 80.0f);
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
	glDrawArrays(GL_TRIANGLES, 0, 36);
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
	if (directionLightOn)
		directionalIntensity = 0.4f;
	else
		directionalIntensity = 0.0f;
	shader.setFloat("dirLight.intensity", directionalIntensity);
	shader.setVec3("dirLight.direction", vec3(-1.0f, -1.0f, -1.0f));
	shader.setVec3("dirLight.ambient", vec3(0.2f, 0.2f, 0.2f));
	shader.setVec3("dirLight.diffuse", vec3(0.4f, 0.4f, 0.4f));
	shader.setVec3("dirLight.specular", vec3(0.5f, 0.5f, 0.5f));

	//Point Light:  Distance:50
	shader.setVec3("pointLight.position", vec3(pointLightX, pointLightY, pointLightZ));
	shader.setVec3("pointLight.ambient", 1.0f * vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("pointLight.diffuse", 2.0f * vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("pointLight.specular", 5.0f * vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("pointLight.constant", pointLightConstant);
	shader.setFloat("pointLight.linear", pointLightLinear);
	shader.setFloat("pointLight.quadratic", pointLightQudratic);
	if (pointLightOn)
		pointLightIntensity = 1.0f;
	else
		pointLightIntensity = 0.0f;
	shader.setFloat("pointLight.intensity", pointLightIntensity);

	//Send camera information to the shader:
	shader.setVec3("eyePositionWorld", cameraPosition);

	//Draw Spacecraft:
	modelMatrix = mat4(1.0f);
	modelMatrix = translate(modelMatrix, cameraPosition);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-yaw - 90),
		glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(pitch),
		glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = translate(modelMatrix, vec3(0.0f, -camera_offset_Y, -camera_offset_Z));
	modelMatrix = rotate(modelMatrix, (float)radians(180.0), vec3(0.0, 1.0, 0.0));
	modelMatrix = scale(modelMatrix, vec3(scaleFactor * 0.0025, scaleFactor * 0.0025, scaleFactor * 0.0025));
	shader.setMat4("model", modelMatrix);
	spacecraftTexture[goldCollected == NUM_GOLD].bind(0);
	shader.setInt("texureSampler0", 0);
    shader.setInt("normalMapping_flag", 0);
	glBindVertexArray(spacecraftVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spacecraftEBO);
	glDrawElements(GL_TRIANGLES, Spacecraft.indices.size(), GL_UNSIGNED_INT, 0);
	// Regard the central point as the object position
	glm::vec4 spft_vec = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glBindVertexArray(0);

    //Draw Planet:
    modelMatrix = mat4(1.0f);
    modelMatrix = scale(modelMatrix, vec3(scaleFactor * 4.5f, scaleFactor * 4.5f, scaleFactor * 4.5f));
    modelMatrix = rotate(modelMatrix, (float)(glfwGetTime() * 0.1f), vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = rotate(modelMatrix, (float)radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = translate(modelMatrix, vec3(0.0f, -1.04894f, 0.0f));
    shader.setMat4("model", modelMatrix);
    planetTexture.bind(0);
    shader.setInt("textureSampler0", 0);
    planetNormal.bind(1);
    shader.setInt("textureSampler1", 1);
    shader.setInt("normalMapping_flag", 1);
    glBindVertexArray(planetVAO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planetEBO);
    //glDrawElements(GL_TRIANGLES, Planet.indices.size(), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, Planet.vertices_without_indices.size());

	//Draw Local Crafts:
	modelMatrix = mat4(1.0f);
	modelMatrix = rotate(modelMatrix, rotate_speed * (float)glfwGetTime()*1.5f, vec3(0.0, 1.0, 0.0));
	modelMatrix = translate(modelMatrix, vec3(0.0f, -0.1f, 8.0f));
	modelMatrix = rotate(modelMatrix, 0.5f * (float)glfwGetTime(), vec3(0.0, 1.0, 0.0));
	modelMatrix = scale(modelMatrix, vec3(scaleFactor * 0.3, scaleFactor * 0.3, scaleFactor * 0.3));
	shader.setMat4("model", modelMatrix);
	glBindVertexArray(craftVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, craftEBO);
	// Regard the central point as the object position
	glm::vec4 loft_vec = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (collision_detection(vec3(spft_vec), vec3(loft_vec), spft_dim, loft_dim) == 1) {
		collision_near = true;
		last_time = (float)glfwGetTime();
	}
	else
		if ((float)glfwGetTime() - last_time > 2)
			collision_near = false;
	craftTexture[collision_near].bind(0);
	shader.setInt("texureSampler0", 0);
    shader.setInt("normalMapping_flag", 0);
	glDrawElements(GL_TRIANGLES, Craft.indices.size(), GL_UNSIGNED_INT, 0);

	//Middle one:
	modelMatrix = mat4(1.0f);
	modelMatrix = rotate(modelMatrix, rotate_speed * (float)glfwGetTime()*2.0f, vec3(0.0, 1.0, 0.0));
	modelMatrix = translate(modelMatrix, vec3(4.0f, -0.1f, 9.0f));
	modelMatrix = rotate(modelMatrix, 0.5f * (float)glfwGetTime(), vec3(0.0, 1.0, 0.0));
	modelMatrix = scale(modelMatrix, vec3(scaleFactor * 0.3, scaleFactor * 0.3, scaleFactor * 0.3));
	shader.setMat4("model", modelMatrix);
	glBindVertexArray(craftVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, craftEBO);
	// Regard the central point as the object position
	loft_vec = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (collision_detection(vec3(spft_vec), vec3(loft_vec), spft_dim, loft_dim) == 1) {
		collision_middle = true;
		last_time = (float)glfwGetTime();
	}
	else
		if ((float)glfwGetTime() - last_time > 2)
			collision_middle = false;
	craftTexture[collision_middle].bind(0);
	shader.setInt("texureSampler0", 0);
    shader.setInt("normalMapping_flag", 0);
	glDrawElements(GL_TRIANGLES, Craft.indices.size(), GL_UNSIGNED_INT, 0);

	//Far one:
	modelMatrix = mat4(1.0f);
	modelMatrix = rotate(modelMatrix, rotate_speed * (float)glfwGetTime() * 4.0f, vec3(0.0, 1.0, 0.0));
	modelMatrix = translate(modelMatrix, vec3(7.0f, -0.1f, 10.0f));
	modelMatrix = rotate(modelMatrix, 0.5f * (float)glfwGetTime(), vec3(0.0, 1.0, 0.0));
	modelMatrix = scale(modelMatrix, vec3(scaleFactor * 0.3, scaleFactor * 0.3, scaleFactor * 0.3));
	shader.setMat4("model", modelMatrix);
	glBindVertexArray(craftVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, craftEBO);
	// Regard the central point as the object position
	loft_vec = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (collision_detection(vec3(spft_vec), vec3(loft_vec), spft_dim, loft_dim) == 1) {
		collision_far = true;
		last_time = (float)glfwGetTime();
	}
	else
		if ((float)glfwGetTime() - last_time > 2)
			collision_far = false;
	craftTexture[collision_far].bind(0);
	shader.setInt("texureSampler0", 0);
    shader.setInt("normalMapping_flag", 0);
	glDrawElements(GL_TRIANGLES, Craft.indices.size(), GL_UNSIGNED_INT, 0);

	//Horizontal near:
	holdTime[0] = glfwGetTime() - lastTime[0];
	if (holdTime[0] >= 2.0f) {
		if (horizontal_offset[0] >= 6.0f)
			horizontal_offset[0] = 0.0f;
		else
			horizontal_offset[0] += 2.0f;
		lastTime[0] = glfwGetTime();
	}
	modelMatrix = mat4(1.0f);
	modelMatrix = translate(modelMatrix, vec3(-4.0f+horizontal_offset[0], -0.1f, 10.0f));
	modelMatrix = rotate(modelMatrix, 0.5f * (float)glfwGetTime(), vec3(0.0, 1.0, 0.0));
	modelMatrix = scale(modelMatrix, vec3(scaleFactor * 0.3, scaleFactor * 0.3, scaleFactor * 0.3));
	shader.setMat4("model", modelMatrix);
	glBindVertexArray(craftVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, craftEBO);
	// Regard the central point as the object position
	loft_vec = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (collision_detection(vec3(spft_vec), vec3(loft_vec), spft_dim, loft_dim) == 1) {
		collision_horizontal_near = true;
		last_time = (float)glfwGetTime();
	}
	else
		if ((float)glfwGetTime() - last_time > 2)
			collision_horizontal_near = false;
	craftTexture[collision_horizontal_near].bind(0);
	shader.setInt("texureSampler0", 0);
    shader.setInt("normalMapping_flag", 0);
	glDrawElements(GL_TRIANGLES, Craft.indices.size(), GL_UNSIGNED_INT, 0);

	//Horizontal Middle
	holdTime[1] = glfwGetTime() - lastTime[1];
	if (holdTime[1] >= 1.8f) {
		if (horizontal_offset[1] >= 10.0f)
			horizontal_offset[1] = 0.0f;
		else
			horizontal_offset[1] += 2.5f;
		lastTime[1] = glfwGetTime();
	}
	modelMatrix = mat4(1.0f);
	modelMatrix = translate(modelMatrix, vec3(-5.0f + horizontal_offset[1], -0.1f, 11.0f));
	modelMatrix = rotate(modelMatrix, 0.5f * (float)glfwGetTime(), vec3(0.0, 1.0, 0.0));
	modelMatrix = scale(modelMatrix, vec3(scaleFactor * 0.3, scaleFactor * 0.3, scaleFactor * 0.3));
	shader.setMat4("model", modelMatrix);
	glBindVertexArray(craftVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, craftEBO);
	// Regard the central point as the object position
	loft_vec = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (collision_detection(vec3(spft_vec), vec3(loft_vec), spft_dim, loft_dim) == 1) {
		collision_horizontal_middle = true;
		last_time = (float)glfwGetTime();
	}
	else
		if ((float)glfwGetTime() - last_time > 2)
			collision_horizontal_middle = false;
	craftTexture[collision_horizontal_middle].bind(0);
	shader.setInt("texureSampler0", 0);
    shader.setInt("normalMapping_flag", 0);
	glDrawElements(GL_TRIANGLES, Craft.indices.size(), GL_UNSIGNED_INT, 0);

	//Horizontal Far
	holdTime[2] = glfwGetTime() - lastTime[2];
	if (holdTime[2] >= 1.5f) {
		if (horizontal_offset[2] >= 12.0f)
			horizontal_offset[2] = 0.0f;
		else
			horizontal_offset[2] += 3.0f;
		lastTime[2] = glfwGetTime();
	}
	modelMatrix = mat4(1.0f);
	modelMatrix = translate(modelMatrix, vec3(-6.0f + horizontal_offset[2], -0.1f, 14.0f));
	modelMatrix = rotate(modelMatrix, 0.5f * (float)glfwGetTime(), vec3(0.0, 1.0, 0.0));
	modelMatrix = scale(modelMatrix, vec3(scaleFactor * 0.3, scaleFactor * 0.3, scaleFactor * 0.3));
	shader.setMat4("model", modelMatrix);
	glBindVertexArray(craftVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, craftEBO);
	// Regard the central point as the object position
	loft_vec = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (collision_detection(vec3(spft_vec), vec3(loft_vec), spft_dim, loft_dim) == 1) {
		collision_horizontal_far = true;
		last_time = (float)glfwGetTime();
	}
	else
		if ((float)glfwGetTime() - last_time > 2)
			collision_horizontal_far = false;
	craftTexture[collision_horizontal_far].bind(0);
	shader.setInt("texureSampler0", 0);
    shader.setInt("normalMapping_flag", 0);
	glDrawElements(GL_TRIANGLES, Craft.indices.size(), GL_UNSIGNED_INT, 0);


	//Draw Rock Ring:
	for (int i = 0; i < NUM_BLOCK; i++) {
		float X = cos(rockRing->theta[i]) * rockRing->distance[i];
		float Z = sin(rockRing->theta[i]) * rockRing->distance[i];
		modelMatrix = mat4(1.0f);
		modelMatrix = rotate(modelMatrix, rockRing->rotationSpeed * (float)glfwGetTime(), vec3(0.0, 1.0, 0.0));
		modelMatrix = translate(modelMatrix, vec3(X, rockRing->Y[i], Z));
		modelMatrix = rotate(modelMatrix, (float)radians(glfwGetTime()) * rockRing->selfRotationSpeed, vec3(1.0, 0.0, 1.0));
		modelMatrix = scale(modelMatrix, vec3(scaleFactor * rockRing->scale[i], scaleFactor * rockRing->scale[i], scaleFactor * rockRing->scale[i]));
		glm::vec4 rock_vec = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		if (rockRing->isGold[i]) {
			if (collision_detection(spft_vec, vec3(rock_vec), spft_dim, rock_dim) == 1) {
				rockRing->isGold[i] = false;
				cout << "Detection!" << endl;
				goldCollected += 1;
			}
		}
		shader.setMat4("model", modelMatrix);
		rockTexture[rockRing->isGold[i]].bind(0);
		shader.setInt("texureSampler0", 0);
        shader.setInt("normalMapping_flag", 0);
		glBindVertexArray(rockVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rockEBO);
		glDrawElements(GL_TRIANGLES, Rock.indices.size(), GL_UNSIGNED_INT, 0);
	}
	//Draw Coin:
	modelMatrix = mat4(1.0f);
	modelMatrix = translate(modelMatrix, vec3(-10.0f, -0.1f, 18.0f));
	modelMatrix = rotate(modelMatrix, 0.5f * (float)glfwGetTime(), vec3(1.0, 0.0, 1.0));
	modelMatrix = scale(modelMatrix, vec3(scaleFactor*18.0, scaleFactor * 18.0, scaleFactor * 18.0));
	shader.setMat4("model", modelMatrix);
	glBindVertexArray(coinVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coinEBO);
	// Regard the central point as the object position
	loft_vec = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	if (collision_detection(vec3(spft_vec), vec3(loft_vec), spft_dim, loft_dim) == 1&&speedMode!=true) {
		cout << "Coin collected! Speed mode unlocked!" << endl;
		speedMode = true;
	}
	if(!speedMode){
		coinTexture.bind(0);
		shader.setInt("texureSampler0", 0);
        shader.setInt("normalMapping_flag", 0);
		glDrawElements(GL_TRIANGLES, Coin.indices.size(), GL_UNSIGNED_INT, 0);
	}
	
	//Draw Rocket:
	for (int i = 0; i < 2; i++) {
		modelMatrix = mat4(1.0f);
		if (rockets[i].state == 0) {
			modelMatrix = glm::translate(modelMatrix, rockets[i].initial_position);
			modelMatrix = glm::rotate(modelMatrix, float(glfwGetTime()), vec3(0.0f, 1.0f, 1.0f));
			modelMatrix = glm::rotate(modelMatrix, (float)radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
		}
		else if (rockets[i].state == 1) {
			modelMatrix = glm::translate(modelMatrix, cameraPosition);
			modelMatrix = glm::rotate(modelMatrix, glm::radians(-yaw - 90),
				glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(pitch),
				glm::vec3(1.0f, 0.0f, 0.0f));
			if (rockets[i].loaded_position == 1)
				modelMatrix = glm::translate(modelMatrix, vec3(-0.3f, -camera_offset_Y - 0.1f, -camera_offset_Z - 0.4f));
			if (rockets[i].loaded_position == 2)
				modelMatrix = glm::translate(modelMatrix, vec3(0.3f, -camera_offset_Y - 0.1f, -camera_offset_Z - 0.4f));
			modelMatrix = glm::rotate(modelMatrix, float(radians(90.0f)), vec3(0.0f, 1.0f, 0.0f));
		}
		else if (rockets[i].state == 2) {
			if (glfwGetTime() - rockets[i].timer > 2)
				rockets[i].state = 0;
			if (rockets[i].fire_first_determine) {
				glm::mat4 firemodelMatrix = glm::translate(glm::mat4(1.0f), cameraPosition);
				firemodelMatrix = glm::rotate(firemodelMatrix, glm::radians(-yaw - 90),
					glm::vec3(0.0f, 1.0f, 0.0f));
				firemodelMatrix = glm::rotate(firemodelMatrix, glm::radians(pitch),
					glm::vec3(1.0f, 0.0f, 0.0f));
				if (rockets[i].loaded_position == 1)
					firemodelMatrix = glm::translate(firemodelMatrix, vec3(-0.3f, -camera_offset_Y - 0.1f, -camera_offset_Z - 0.4f));
				if (rockets[i].loaded_position == 2)
					firemodelMatrix = glm::translate(firemodelMatrix, vec3(0.3f, -camera_offset_Y - 0.1f, -camera_offset_Z - 0.4f));
				firemodelMatrix = glm::rotate(firemodelMatrix, float(radians(90.0f)), vec3(0.0f, 1.0f, 0.0f));
				rockets[i].fire_position_matrix = firemodelMatrix;
				rockets[i].fire_direction = targetDirection;
				rockets[i].fire_first_determine = false;
			}
			modelMatrix = glm::translate(modelMatrix, rocket_speed * rockets[i].fire_direction);
			modelMatrix = modelMatrix * rockets[i].fire_position_matrix;
			rockets[i].fire_position_matrix = modelMatrix;
		}
		modelMatrix = glm::scale(modelMatrix, vec3(scaleFactor * 0.5f, scaleFactor * 0.5f, scaleFactor * 0.5f));
		glm::vec4 rocket_vec = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		if (rockets[i].state == 2 && collision_detection(vec3(loft_vec), vec3(rocket_vec), loft_dim, rocket_dim) == 1) {
			collision_near = true;
			last_time = (float)glfwGetTime();
			rockets[i].state = 0;
		}
		if (rockets[i].state == 0 && collision_detection(spft_vec, glm::vec3(rocket_vec), spft_dim, rocket_dim) == 1) {
			rockets[i].state = 1;
			rockets_loaded += 1;
			if (rockets_loaded == 1) {
				rockets[i].loaded_position = 1;
				left_loaded = true;
			}
			else {
				if (left_loaded) {
					rockets[i].loaded_position = 2;
					right_loaded = true;
				}
				else {
					rockets[i].loaded_position = 1;
					left_loaded = true;
				}
			}
		}
		shader.setMat4("model", modelMatrix);
		rocketTexture.bind(0);
		shader.setInt("texureSampler0", 0);
        shader.setInt("normalMapping_flag", 0);
		glBindVertexArray(rocketVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rocketEBO);
		glDrawElements(GL_TRIANGLES, Rocket.indices.size(), GL_UNSIGNED_INT, 0);
	}
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
	if (firstMouse)
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

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
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
	if (key == GLFW_KEY_2 && action == GLFW_PRESS && speedMode == true) {
		speedModeOn = speedModeOn ^ true;
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
		directionLightOn = directionLightOn ^ true;
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
		pointLightOn = pointLightOn ^ true;
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		if (rockets_loaded == 0) return;
		rockets_loaded -= 1;
		if (rockets_loaded == 1) {
			for (int i = 0; i < 2; i++) {
				if (rockets[i].state == 1 && rockets[i].loaded_position == 1) {
					rockets[i].state = 2;
					rockets[i].fire_first_determine = true;
					rockets[i].timer = glfwGetTime();
					left_loaded = false;
				}
			}
		}
		else {
			for (int i = 0; i < 2; i++) {
				if (rockets[i].state == 1) {
					rockets[i].state = 2;
					rockets[i].fire_first_determine = true;
					rockets[i].timer = glfwGetTime();
					left_loaded = false;
					right_loaded = false;
					break;
				}
			}
		}
	}
}


int main(int argc, char* argv[])
{
	GLFWwindow* window;
	decidePosition(rockRing);
	for (int i = 0; i < 3; i++) {
		lastTime[0]=lastTime[1]=lastTime[2] = glfwGetTime();
	}
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
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		spmv.speed = 4.5f * deltaTime;
        rocket_speed = 25.0f * deltaTime;
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

void decidePosition(struct RockRing* rockRing) {
	random_device rd;
	default_random_engine eng(rd());
	uniform_real_distribution<> distr(0.0f, 360.0f);
	uniform_real_distribution<> dista(4.0f, 5.5f);
	uniform_real_distribution<> disty(-0.3f, 0.3f);
	cout << rockRing->currentNumber<<endl;
	for (int i = 0; i < NUM_BLOCK; i++) {
		if (rockRing->currentNumber <= NUM_GOLD) 
			rockRing->isGold[i] = true;
		
		else
			rockRing->isGold[i] = false;
		rockRing->distance[i] = dista(eng);
		rockRing->theta[i] = distr(eng);
		rockRing->Y[i] = disty(eng);
		rockRing->currentNumber++;
		rockRing->scale[i] = (rand() % 5 + 5) / 30.0f;
	}
}
