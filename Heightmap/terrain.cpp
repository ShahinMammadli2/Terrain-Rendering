#include "terrain.h"

Terrain::~Terrain()
{
	if (texture) glDeleteTextures(1, &texture);
	if (groundTex) glDeleteTextures(1, &groundTex);
	if (VBO) glDeleteBuffers(1, &VBO);
	if (VAO) glDeleteVertexArrays(1, &VAO);
}

bool Terrain::LoadHeightmap(const char* path)
{
	if (texture = CreateTexture(path); !texture)
	{
		std::cerr << "Failed to load heightmap" << path << std::endl;
		return false;
	}
	return true;
}

bool Terrain::LoadTexture(const char* path)
{
	if (groundTex = CreateTextureGround(path);!groundTex)
	{
		std::cerr << "Failed to load texture" << path << std::endl;
		return false;
	}
	return true;
}

void Terrain::Build()
{
	GenerateVertices();
	Init();
}

void Terrain::GenerateVertices()
{
	vertices.clear();
	vertices.reserve(rez * rez * 4 * 5);

	for (uint32_t i = 0; i < rez; i++)
	{
		for (uint32_t j = 0; j < rez; j++)
		{
			// (i, j)
			vertices.emplace_back(-terrainWidth * 0.5f + terrainWidth * i / (float)rez);
			vertices.emplace_back(0.0f);
			vertices.emplace_back(-terrainHeight * 0.5f + terrainHeight * j / (float)rez);
			vertices.emplace_back(i / (float)rez);
			vertices.emplace_back(j / (float)rez);

			// (i+1, j)
			vertices.emplace_back(-terrainWidth * 0.5f + terrainWidth * (i + 1) / (float)rez);
			vertices.emplace_back(0.0f);
			vertices.emplace_back(-terrainHeight * 0.5f + terrainHeight * j / (float)rez);
			vertices.emplace_back((i + 1) / (float)rez);
			vertices.emplace_back(j / (float)rez);

			// (i+1, j+1)
			vertices.emplace_back(-terrainWidth * 0.5f + terrainWidth * (i + 1) / (float)rez);
			vertices.emplace_back(0.0f);
			vertices.emplace_back(-terrainHeight * 0.5f + terrainHeight * (j + 1) / (float)rez);
			vertices.emplace_back((i + 1) / (float)rez);
			vertices.emplace_back((j + 1) / (float)rez);

			// (i, j+1)
			vertices.emplace_back(-terrainWidth * 0.5f + terrainWidth * i / (float)rez);
			vertices.emplace_back(0.0f);
			vertices.emplace_back(-terrainHeight * 0.5f + terrainHeight * (j + 1) / (float)rez);
			vertices.emplace_back(i / (float)rez);
			vertices.emplace_back((j + 1) / (float)rez);
		}
	}
}

uint32_t Terrain::CreateTexture(const std::string& path)
{
	stbi_set_flip_vertically_on_load(0);

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "stbi_load failed for " << path
			<< " reason: " << stbi_failure_reason() << std::endl;
		return 0;
	}

	GLenum internalFormat = GL_RGB;
	GLenum dataFormat = GL_RGB;

	if (nrChannels == 1)
	{
		internalFormat = GL_RED;
		dataFormat = GL_RED;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}
	else if (nrChannels == 3)
	{
		internalFormat = GL_RGB;
		dataFormat = GL_RGB;
	}
	else if (nrChannels == 4)
	{
		internalFormat = GL_RGBA;
		dataFormat = GL_RGBA;
	}
	else
	{
		std::cerr << "Unsupported channel count: " << nrChannels << std::endl;
		stbi_image_free(data);
		return 0;
	}

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (nrChannels == 1)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // restore default

	stbi_image_free(data);
	return texture;
}

uint32_t Terrain::CreateTextureGround(const std::string& path)
{
	stbi_set_flip_vertically_on_load(0);

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "stbi_load failed for " << path
			<< " reason: " << stbi_failure_reason() << std::endl;
		return 0;
	}

	GLenum internalFormat = GL_RGB;
	GLenum dataFormat = GL_RGB;

	if (nrChannels == 1)
	{
		internalFormat = GL_RED;
		dataFormat = GL_RED;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}
	else if (nrChannels == 3)
	{
		internalFormat = GL_RGB;
		dataFormat = GL_RGB;
	}
	else if (nrChannels == 4)
	{
		internalFormat = GL_RGBA;
		dataFormat = GL_RGBA;
	}
	else
	{
		std::cerr << "Unsupported channel count: " << nrChannels << std::endl;
		stbi_image_free(data);
		return 0;
	}

	glGenTextures(1, &groundTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (nrChannels == 1)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // restore default

	stbi_image_free(data);
	return groundTex;
}

void Terrain::DrawTerrain(Shader& shader, glm::mat4& model, glm::mat4& view, glm::mat4& projection) const 
{
	shader.Use();

	// Matrices
	shader.SetMat4("model", model);
	shader.SetMat4("view", view);
	shader.SetMat4("projection", projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	shader.setInteger("heightMap", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->groundTex);
	shader.setInteger("terrainTex", 1);

	// Bind VAO and draw as patches
	glBindVertexArray(this->VAO);
	GLsizei vertexCount = static_cast<GLsizei>(vertices.size() / 5); // 5 = 3 pos + 2 uv
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, vertexCount);
	glBindVertexArray(0);
}

void Terrain::Init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
}