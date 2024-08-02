#pragma once

#include<GL/glew.h>
#include<stb_image.h>
#include<iostream>
#include"Helper.h"


struct Texture
{
		GLuint id;
		std::string type;
		std::string path;
        
        void GenerateTexture();
        void SetParameters(std::string type);
        void AttachShadowImage(GLenum format, int width, int height);
        void AttachCubeMapImage(GLuint i, int width, int height, unsigned char* data);
        void AttachImage(GLenum format, int width, int height, unsigned char* data);
        void LoadTextureFromFile(const char* path, const std::string& directory);
        void ActiveTexture(GLuint i);
        void Bind(GLenum format);
        void Unbind(GLenum format);
};

void Texture::GenerateTexture() {
    glGenTextures(1, &id);
}

void Texture::AttachShadowImage(GLenum format, int width, int height) {
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, NULL);
}

void Texture::AttachCubeMapImage(GLuint i, int width, int height, unsigned char* data) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void Texture::AttachImage(GLenum format, int width, int height, unsigned char* data) {
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
}

void Texture::Bind(GLenum format = GL_TEXTURE_2D) {
    glBindTexture(format, id);
}

void Texture::Unbind(GLenum format = GL_TEXTURE_2D) {
    glBindTexture(format, 0);
}

void Texture::ActiveTexture(GLuint i) {
    glActiveTexture(GL_TEXTURE0 + i);
}

void Texture::SetParameters(std::string type = "texture") {
    if (type == "texture") {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if (type == "shadowMap") {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else if (type == "cubeMap") {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
}

void Texture::LoadTextureFromFile(const char* path, const std::string& directory) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        Bind();
        AttachImage(format, width, height, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        SetParameters();
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
}
