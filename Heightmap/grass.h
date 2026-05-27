#ifndef GRASS_H
#define GRASS_H

#include "shader.h"
#include "texture.h"
#include "heightmap_png.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Grass {
public:
	Grass(Shader& shader);
	~Grass();
	void Draw(Texture2D& texture, glm::mat4& view, glm::mat4& projection);

private:
	unsigned int VAO, EBO, VBO, instanceVBO;

	int amount = 10000;

	Shader shader;
	HeightMapPNG noise;

	float RandomFloat(float min, float max);

	void Init();
};

#endif // !GRASS_H
