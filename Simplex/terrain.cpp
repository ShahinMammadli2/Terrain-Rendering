#include "terrain.h"

Terrain::~Terrain()
{
	if (texture) glDeleteTextures(1, &texture);
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
	if (texture = CreateTexture(path);!texture)
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
	float tile{ 20.0f };
	vertices.reserve(rez * rez * 4 * 5);
	for (uint32_t i = 0; i <= rez - 1; i++)
	{
		for (uint32_t j = 0; j <= rez - 1; j++)
		{
			// (i,j)
			vertices.emplace_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.emplace_back(0.0f); // v.y;
			vertices.emplace_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.emplace_back(i / (float)rez * tile); // u
			vertices.emplace_back(j / (float)rez * tile); // v

			// (i+1,j)
			vertices.emplace_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.emplace_back(0.0f); // v.y
			vertices.emplace_back(-height / 2.0f + height * j / (float)rez);
			vertices.emplace_back((i + 1) / (float)rez * tile); // u
			vertices.emplace_back(j / (float)rez * tile); // v

			// (i+1,j+1)
			vertices.emplace_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.emplace_back(0.0f); // v.y
			vertices.emplace_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.emplace_back((i + 1) / (float)rez * tile); // u
			vertices.emplace_back((j + 1) / (float)rez * tile); // v

			// (i,j+1)
			vertices.emplace_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.emplace_back(0.0f); // v.y
			vertices.emplace_back(-height / 2.0f + height * (j + 1) / (float)rez);
			vertices.emplace_back(i / (float)rez * tile); // u
			vertices.emplace_back((j + 1) / (float)rez * tile); // v
		}
	}
}

uint32_t Terrain::CreateTexture(const std::string& path)
{
	stbi_set_flip_vertically_on_load(0);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "stbi_load failed for '" << path << "' reason: " << stbi_failure_reason() << std::endl;
		return 0;
	}
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return texture;
}

void Terrain::DrawTerrain(Shader& shader, glm::mat4& model, glm::mat4& view, glm::mat4& projection) const 
{
	shader.Use();

	// Matrices
	shader.SetMat4("uModel", model);
	shader.SetMat4("uView", view);
	shader.SetMat4("uProj", projection);
	shader.setFloat("uMinDistance", 30.0f);
	shader.setFloat("uMaxDistance", 600.0f);
	shader.setFloat("uMinTess", 4.0f);
	shader.setFloat("uMaxTess", 32.0f);

	// Tess factors
	shader.setFloat("uTessInner", 8.0f);
	shader.setFloat("uTessOuter", 8.0f);

	// Perlin/FBM params
	shader.setFloat("uAmplitude", 40.0f);   
	shader.setFloat("uFrequency", 1.0 / 180.0);   
	shader.setInteger("uOctaves", 6);
	shader.setFloat("uLacunarity", 2.0f);
	shader.setFloat("uGain", 0.48f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	shader.setInteger("uTerrainTex", 0);

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