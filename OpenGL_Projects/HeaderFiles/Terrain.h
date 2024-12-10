#pragma once

#include<glm/glm.hpp>
#include<stb_image.h>
#include<vector>
#include"Vertex.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Texture.h"

std::string directory = "./../../../Resources/Textures/";

class Terrain {
    public:
    Terrain(std::string heightmap);
    glm::mat4 model = glm::mat4(1.0f);
    void Draw(Shader &shader);

    private:
    void CreateMesh();
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    Texture texture;
    std::string heightmapSource;
    int numStrips, numTrisPerStrip;
    VAO vao;
    VBO vbo;
    EBO ebo;
};

Terrain::Terrain(std::string heightmap) {
    heightmapSource = heightmap;
    model = glm::mat4(1.0f);
    CreateMesh();
    // first, configure the cube's VAO (and terrainVBO + terrainIBO)
    vao.Bind();
    vbo.Bind();
    vbo.AttachData(vertices);
    vao.LinkVBO(vbo, 0, 3, GL_FLOAT, (void*)0);
    vao.LinkVBO(vbo, 1, 3, GL_FLOAT, (void*)offsetof(Vertex, normals));
	vao.LinkVBO(vbo, 2, 2, GL_FLOAT, (void*)offsetof(Vertex, textures));
    ebo.Bind();
    ebo.AttachData(indices);

    // Texture
	texture.GenerateTexture();
	texture.Bind();
	texture.SetParameters("heightMap");
	texture.LoadTextureFromFile("WildGrass.png" , directory);
}


void Terrain::CreateMesh() {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load((directory + heightmapSource + ".png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float yScale = 0.1 ,yShift = 15.0f;
    int rez = 1;
    unsigned bytePerPixel = nrChannels;
    
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            unsigned char* pixelOffset = data + (j + width * i) * bytePerPixel;
            unsigned char y = pixelOffset[0];
            glm::vec3 v;
            glm::vec2 t;
            Vertex vert;
            // vertex
            v.x = -height/2.0f + height*i/(float)height;
            v.y = (int) y * yScale - yShift;
            v.z = -width/2.0f + width*j/(float)width;

            t.x = (float) j / width;
            t.y = (float) i / height;

            vert.positions = v;
            vert.textures = t;
            vertices.push_back(vert);
        }
    }
    std::cout << "Loaded " << vertices.size() / 3 << " vertices" << std::endl;
    stbi_image_free(data);

    for(unsigned i = 0; i < height-1; i += rez)
    {
        for(unsigned j = 0; j < width; j += rez)
        {
            for(unsigned k = 0; k < 2; k++)
            {
                indices.push_back(j + width * (i + k*rez));
            }
        }
    }
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;

    numStrips = (height-1)/rez;
    numTrisPerStrip = (width/rez)*2-2;
    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;
}

void Terrain::Draw(Shader &shader) {
    texture.Bind();
	texture.ActiveTexture(0);
    shader.setInt("terrainTexture", 0);
    vao.Bind();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for(unsigned strip = 0; strip < numStrips; strip++) {
        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
            numTrisPerStrip+2,   // number of indices to render
            GL_UNSIGNED_INT,     // index data type
            (void*)(sizeof(unsigned) * (numTrisPerStrip+2) * strip)); // offset to starting index
        }
}