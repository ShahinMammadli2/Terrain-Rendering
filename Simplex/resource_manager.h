#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "texture.h"
#include "shader.h"

class ResourceManager {
public:
	// resource storage
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D> Textures;
	static Shader& LoadShader(const char* vShaderFile, const char* fShaderFile, const char* tcFile, const char* teFile, const char* gShaderFile, std::string name);
	static Shader& GetShader(std::string name);
	static Texture2D& LoadTexture(const char* file, bool alpha, std::string name);
	static Texture2D& GetTexture(std::string name);
	static void Clear();
private:
	ResourceManager() {}
	static Shader LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* tcFile = nullptr, const char* teFile = nullptr, const char* gShaderFile = nullptr);
	static Texture2D LoadTextureFromFile(const char* file, bool alpha);
};
#endif // !RESOURCE_MANAGER_H
