#ifndef TERRAIN_H
#define TERRAIN_H
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include "shader.h"

class Terrain {
public:
	~Terrain();
	bool LoadHeightmap(const char* path);
	bool LoadTexture(const char* path);
	void DrawTerrain(Shader& shader, glm::mat4& model, glm::mat4& view, glm::mat4& projection) const;
	void Build();
private:
	std::vector<float> vertices;
	uint32_t rez{ 20 };
	float terrainWidth{ 2624.0f };
	float terrainHeight{ 1756.0f };
	uint32_t VAO{};
	uint32_t VBO{};
	int width{ 0 }, height{ 0 }, nrChannels{ 0 };
	uint32_t texture{};
	uint32_t groundTex{};
	void GenerateVertices();
	uint32_t CreateTexture(const std::string& path);
	uint32_t CreateTextureGround(const std::string& path);
	void Init();
};
#endif // !TERRAIN_H
