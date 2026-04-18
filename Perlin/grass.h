#ifndef GRASS_H
#define GRASS_H

#include "shader.h"
#include "texture.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Grass{
public:
	Grass(Shader& shader);
	~Grass();
	void Draw(Texture2D& texture, glm::mat4& view, glm::mat4& projection);

	float uFrequency = 50.0f;
	float uAmplitude = 300.0f;
	float spreadRadius = 500.0f;

private:
	unsigned int VAO, EBO, VBO, instanceVBO;

	int amount = 1000;

	Shader shader;

	float Random(glm::vec2 p);
	glm::vec2 Gradient(glm::vec2 p);
	glm::vec2 Fade(glm::vec2 t);
	float Perlin2D(glm::vec2 p);
	float GetTerrainHeight(float x, float z);

	void Init();
};

#endif // !GRASS_H
