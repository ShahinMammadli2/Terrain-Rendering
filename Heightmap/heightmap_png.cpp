#include "stb_image.h"
#include "stb_image_write.h"

#include "heightmap_png.h"
#include <algorithm>
#include <cmath>

HeightMapPNG::HeightMapPNG()
	:m_Width(0), m_Height(0), m_WorldSizeX(0.0f), m_WorldSizeZ(0.0f), m_Amplitude(0.0f)
{
}

HeightMapPNG::~HeightMapPNG()
{
}

bool HeightMapPNG::GenerateAndSave(const std::string& filename, int width, int height, float worldSizeX, float worldSizeZ, float frequency, float amplitude, const glm::vec2& scroll, NoiseType type)
{
    if (width <= 1 || height <= 1)
        return false;
    m_Width = width;
    m_Height = height;
    m_WorldSizeX = worldSizeX;
    m_WorldSizeZ = worldSizeZ;
    m_Amplitude = amplitude;
    m_Pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height));

    float halfX = worldSizeX * 0.5f;
    float halfZ = worldSizeZ * 0.5f;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            float u = static_cast<float>(x) / static_cast<float>(width - 1);
            float v = static_cast<float>(y) / static_cast<float>(height - 1);

            float worldX = glm::mix(-halfX, halfX, u);
            float worldZ = glm::mix(-halfZ, halfZ, v);

            glm::vec2 p = glm::vec2(worldX, worldZ) * frequency + scroll;

            float n = 0.0f;
            if (type == NoiseType::Perlin)
                n = Perlin2D(p);
            else
                n = Simplex2D(p);

            float normalized = glm::clamp(n * 0.5f + 0.5f, 0.0f, 1.0f);
            unsigned char value = static_cast<unsigned char>(normalized * 255.0f);
            m_Pixels[y * width + x] = value;
        }
    }
    int result = stbi_write_png(filename.c_str(), width, height, 1, m_Pixels.data(), width);
    return result != 0;
}

bool HeightMapPNG::Load(const std::string& filename, float amplitude, float worldSizeX, float worldSizeZ)
{
    int width = 0, height = 0, nrChannels = 0;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 1);
    if (!data)
        return false;

    m_Width = width;
    m_Height = height;
    m_Amplitude = amplitude;
    m_WorldSizeX = worldSizeX;
    m_WorldSizeZ = worldSizeZ;

    m_Pixels.assign(data, data + (width * height));
    stbi_image_free(data);

    return true;
}

float HeightMapPNG::GetHeightWorld(float worldX, float worldZ) const
{
    if (m_Pixels.empty() || m_WorldSizeX <= 0.0f || m_WorldSizeZ <= 0.0f)
        return 0.0f;

    float halfX = m_WorldSizeX * 0.5f;
    float halfZ = m_WorldSizeZ * 0.5f;

    float u = (worldX + halfX) / m_WorldSizeX;
    float v = (worldZ + halfZ) / m_WorldSizeZ;

    u = glm::clamp(u, 0.0f, 1.0f);
    v = glm::clamp(v, 0.0f, 1.0f);

    float px = u * static_cast<float>(m_Width - 1);
    float py = v * static_cast<float>(m_Height - 1);

    int x0 = static_cast<int>(std::floor(px));
    int y0 = static_cast<int>(std::floor(py));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float tx = px - static_cast<float>(x0);
    float ty = py - static_cast<float>(y0);

    float h00 = SampleHeightPixel(x0, y0);
    float h10 = SampleHeightPixel(x1, y0);
    float h01 = SampleHeightPixel(x0, y1);
    float h11 = SampleHeightPixel(x1, y1);

    float hx0 = glm::mix(h00, h10, tx);
    float hx1 = glm::mix(h01, h11, tx);

    return glm::mix(hx0, hx1, ty);
}

glm::vec3 HeightMapPNG::GetNormalWorld(float worldX, float worldZ, float sampleStep) const
{
    float hL = GetHeightWorld(worldX - sampleStep, worldZ);
    float hR = GetHeightWorld(worldX + sampleStep, worldZ);
    float hD = GetHeightWorld(worldX, worldZ - sampleStep);
    float hU = GetHeightWorld(worldX, worldZ + sampleStep);

    return glm::normalize(glm::vec3(hL - hR, 2.0f * sampleStep, hD - hU));
}

float HeightMapPNG::Random(const glm::vec2& p) const
{
	return glm::fract(glm::sin(glm::dot(p, glm::vec2(127.1f, 311.7f))) * 43758.5453f);
}

glm::vec2 HeightMapPNG::Gradient(const glm::vec2& p) const
{
	float r = Random(p) * 6.2831853f;
	return glm::vec2(glm::cos(r), glm::sin(r));
}

glm::vec2 HeightMapPNG::Fade(const glm::vec2& t) const
{
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float HeightMapPNG::Perlin2D(const glm::vec2& p) const
{
    glm::vec2 i = floor(p);
    glm::vec2 f = fract(p);

    glm::vec2 g00 = Gradient(i + glm::vec2(0.0, 0.0));
    glm::vec2 g01 = Gradient(i + glm::vec2(0.0, 1.0));
    glm::vec2 g10 = Gradient(i + glm::vec2(1.0, 0.0));
    glm::vec2 g11 = Gradient(i + glm::vec2(1.0, 1.0));

    glm::vec2 d00 = (f - glm::vec2(0.0, 0.0));
    glm::vec2 d10 = (f - glm::vec2(1.0, 0.0));
    glm::vec2 d01 = (f - glm::vec2(0.0, 1.0));
    glm::vec2 d11 = (f - glm::vec2(1.0, 1.0));

    float v00 = glm::dot(g00, d00);
    float v10 = glm::dot(g10, d10);
    float v01 = glm::dot(g01, d01);
    float v11 = glm::dot(g11, d11);

    glm::vec2 u = Fade(f);
    float nx0 = glm::mix(v00, v10, u.x);
    float nx1 = glm::mix(v01, v11, u.x);

    return glm::mix(nx0, nx1, u.y);
}

float HeightMapPNG::Simplex2D(const glm::vec2& p) const
{
    float n0, n1, n2; // Noise contributions from the three corners
    const float F2 = 0.5f * (glm::sqrt(3.0f) - 1.0f);
    float s = glm::dot(p, glm::vec2(F2));
    int i = int(glm::floor(p.x + s));
    int j = int(glm::floor(p.y + s));

    const float G2 = (3.0f - glm::sqrt(3.0f)) / 6.0f;
    float t = float(i + j) * G2;
    float X0 = i - t; // Unskew the cell origin back to (x,y) space
    float Y0 = j - t;
    float x0 = p.x - X0; // The x,y distances from the cell origin
    float y0 = p.y - Y0;

    int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0) { i1 = 1; j1 = 0; } // lower triangle
    else { i1 = 0; j1 = 1; }      // upper triangle

    float x1 = x0 - float(i1) + G2; // Offsets for middle corner in (x,y) unskewed coords
    float y1 = y0 - float(j1) + G2;
    float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
    float y2 = y0 - 1.0f + 2.0f * G2;

    glm::vec2 g0 = Gradient(glm::vec2(i, j));
    glm::vec2 g1 = Gradient(glm::vec2(i + i1, j + j1));
    glm::vec2 g2 = Gradient(glm::vec2(i + 1, j + 1));

    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 < 0) n0 = 0.0;
    else {
        t0 *= t0;
        n0 = t0 * t0 * glm::dot(g0, glm::vec2(x0, y0));
    }

    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 < 0) n1 = 0.0;
    else {
        t1 *= t1;
        n1 = t1 * t1 * glm::dot(g1, glm::vec2(x1, y1));
    }

    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 < 0) n2 = 0.0;
    else {
        t2 *= t2;
        n2 = t2 * t2 * glm::dot(g2, glm::vec2(x2, y2));
    }

    return 70 * (n0 + n1 + n2);
}

float HeightMapPNG::SampleHeightPixel(int x, int y) const
{
    if (m_Pixels.empty())
        return 0.0f;

    x = std::clamp(x, 0, m_Width - 1);
    y = std::clamp(y, 0, m_Height - 1);

    unsigned char pixel = m_Pixels[y * m_Width + x];
    float normalized = static_cast<float>(pixel) / 255.0f;

    // Convert [0,1] back to roughly [-1,1]
    float signedNoise = normalized * 2.0f - 1.0f;
    return signedNoise * m_Amplitude;
}
