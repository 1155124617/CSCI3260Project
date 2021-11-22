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
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "Dependencies/stb_image/stb_image.h"


struct Vertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
};


struct Model {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};


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

    while (!file.eof()) {
        // process the object file
        char lineHeader[128];
        file >> lineHeader;

        if (strcmp(lineHeader, "v") == 0) {
            // geometric vertices
            glm::vec3 position;
            file >> position.x >> position.y >> position.z;
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



int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
