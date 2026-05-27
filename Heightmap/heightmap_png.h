#ifndef HEIGHTMAP_PNG_H
#define HEIGHTMAP_PNG_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

enum class NoiseType
{
	Perlin,
	Simplex
};


class HeightMapPNG
{
public:
	HeightMapPNG();
	~HeightMapPNG();

	bool GenerateAndSave(const std::string& filename, int width, int height, float worldSizeX, float worldSizeZ, float frequency, float amplitude, const glm::vec2& scroll, NoiseType type);
	bool Load(const std::string& filename, float amplitude, float worldSizeX, float worldSizeZ);

	float GetHeightWorld(float worldX, float worldZ) const;
	glm::vec3 GetNormalWorld(float worldX, float worldZ, float sampleStep = 1.0f) const;



private:
	int m_Width = 0;
	int m_Height = 0;
	float m_WorldSizeX = 0.0f;
	float m_WorldSizeZ = 0.0f;
	float m_Amplitude = 0.0f;

	std::vector<unsigned char> m_Pixels;

	float Random(const glm::vec2& p) const;
	glm::vec2 Gradient(const glm::vec2& p) const;
	glm::vec2 Fade(const glm::vec2& t) const;

	float Perlin2D(const glm::vec2& p) const;
	float Simplex2D(const glm::vec2& p) const;

	float SampleHeightPixel(int x, int y) const;
};

#endif