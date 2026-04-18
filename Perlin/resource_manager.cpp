#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <stb/stb_image.h>

// Instantiate static variables
std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Shader> ResourceManager::Shaders;

Shader& ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* tcFile, const char* teFile, const char* gShaderFile, std::string name)
{
    Shaders[name] = LoadShaderFromFile(vShaderFile, fShaderFile, tcFile, teFile, gShaderFile);
    return Shaders[name];
}

Shader& ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D& ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
    Textures[name] = LoadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D& ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    for (auto &iter : Shaders)
        glDeleteProgram(iter.second.ID);

    for (auto &iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* tcFile, const char* teFile, const char* gShaderFile)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::string tessControlCode;
    std::string tessEvalCode;

    try
    {
        // open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (tcFile != nullptr)
        {
            std::ifstream tessControlFile(tcFile);
            std::stringstream tessControlStream;
            tessControlStream << tessControlFile.rdbuf();
            tessControlFile.close();
            tessControlCode = tessControlStream.str();
        }

        if (teFile != nullptr)
        {
            std::ifstream tessEvalFile(teFile);
            std::stringstream tessEvalStream;
            tessEvalStream << tessEvalFile.rdbuf();
            tessEvalFile.close();
            tessEvalCode = tessEvalStream.str();
        }

        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    const char* tcsShaderCode = tessControlCode.c_str();
    const char* tesShaderCode = tessEvalCode.c_str();

    Shader shader{};
    shader.Compile(vShaderCode, fShaderCode, tcFile != nullptr ? tcsShaderCode : nullptr, teFile != nullptr ? tesShaderCode : nullptr, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::LoadTextureFromFile(const char* file, bool alpha)
{
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    // generate texture
    texture.Generate(width, height, data);
    // free image data;
    stbi_image_free(data);
    return texture;
}
