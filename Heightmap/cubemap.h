#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "stb_image.h"

#include "shader.h"
#include "camera.h"

class Cubemap {
public:
	// texture id
	uint32_t ID;
	uint32_t VAO;
	int Width, Height, nrChannels;

	Cubemap(Shader& shader);
	~Cubemap();

	unsigned int Generate(const std::vector<std::string>& faces);
	void Render(glm::mat4& view, glm::mat4& projection);

	void Bind() const;
private:
	Shader& shader;

	void InitRenderData();
};

#endif // !CUBEMAP_H

