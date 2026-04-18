#include "grass.h"
#include <cmath>
#include <cstdlib>

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 UV;
};

Grass::Grass(Shader& shader)
    : shader(shader), VAO(0), VBO(0), EBO(0), instanceVBO(0)
{
    Init();
}

Grass::~Grass()
{
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (instanceVBO != 0)
        glDeleteBuffers(1, &instanceVBO);
}

void Grass::Draw(Texture2D& texture, glm::mat4& view, glm::mat4& projection)
{
    shader.Use();
    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.ID);

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0, amount);
    glBindVertexArray(0);
}

float Grass::Random(glm::vec2 p)
{
    float dot = p.x * 127.1f + p.y * 311.7f;
    float s = std::sin(dot) * 43758.5453f;
    return s - std::floor(s);
}

glm::vec2 Grass::Gradient(glm::vec2 p) {
    float r = Random(p) * 6.2831853f;
    return glm::vec2(std::cos(r), std::sin(r));
}

glm::vec2 Grass::Fade(glm::vec2 t) {
    // 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float Grass::Perlin2D(glm::vec2 p) {
    glm::vec2 i = glm::floor(p);
    glm::vec2 f = p - i;  // fract

    glm::vec2 g00 = Gradient(i + glm::vec2(0.0f, 0.0f));
    glm::vec2 g10 = Gradient(i + glm::vec2(1.0f, 0.0f));
    glm::vec2 g01 = Gradient(i + glm::vec2(0.0f, 1.0f));
    glm::vec2 g11 = Gradient(i + glm::vec2(1.0f, 1.0f));

    float v00 = glm::dot(g00, f - glm::vec2(0.0f, 0.0f));
    float v10 = glm::dot(g10, f - glm::vec2(1.0f, 0.0f));
    float v01 = glm::dot(g01, f - glm::vec2(0.0f, 1.0f));
    float v11 = glm::dot(g11, f - glm::vec2(1.0f, 1.0f));

    glm::vec2 u = Fade(f);

    float nx0 = glm::mix(v00, v10, u.x);
    float nx1 = glm::mix(v01, v11, u.x);
    return glm::mix(nx0, nx1, u.y);
}

float Grass::GetTerrainHeight(float x, float z) {
    return Perlin2D(glm::vec2(x, z) * uFrequency) * uAmplitude;
}

void Grass::Init()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 baseQuad[4] =
    {
        glm::vec3(-0.5f, 0.0f, 0.0f), // bottom-left
        glm::vec3(0.5f, 0.0f, 0.0f), // bottom-right
        glm::vec3(0.5f, 1.0f, 0.0f), // top-right
        glm::vec3(-0.5f, 1.0f, 0.0f)  // top-left
    };

    glm::vec2 uvs[4] =
    {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f)
    };

    float angles[3] =
    {
        glm::radians(0.0f),
        glm::radians(60.0f),
        glm::radians(120.0f)
    };

    for (int q = 0; q < 3; q++)
    {
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, angles[q], glm::vec3(0.0f, 1.0f, 0.0f));

        unsigned int start = static_cast<unsigned int>(vertices.size());

        for (int i = 0; i < 4; i++)
        {
            glm::vec4 rotatedPos = rotation * glm::vec4(baseQuad[i], 1.0f);
            Vertex V{};
            V.Position = glm::vec3(rotatedPos);
            V.UV = uvs[i];

            vertices.emplace_back(V);
        }

        indices.emplace_back(start + 0);
        indices.emplace_back(start + 1);
        indices.emplace_back(start + 2);

        indices.emplace_back(start + 2);
        indices.emplace_back(start + 3);
        indices.emplace_back(start + 0);
    }

    //glm::mat4* modelMatrices;
    //modelMatrices = new glm::mat4[amount];
    //// to do
    //for (uint32_t i = 0; i < amount; i++)
    //{
    //    glm::mat4 model = glm::mat4(1.0f);


    //}
    std::vector<glm::mat4> modelMatrices(amount);

    uint32_t seed = 42;
    auto lcg = [&]() -> float {
        seed = seed * 1664525u + 1013904223u;
        return (float)(seed & 0xFFFFFF) / (float)0xFFFFFF;  // [0, 1)
        };

    float half = spreadRadius * 0.5f;

    for (int i = 0; i < amount; i++)
    {
        float x = lcg() * spreadRadius - half;
        float z = lcg() * spreadRadius - half;
        float y = GetTerrainHeight(x, z);

        // Random Y rotation so blades don't all face the same way
        float yRot = lcg() * glm::two_pi<float>();

        // Random scale variation (0.8 to 1.2)
        float scale = 0.8f + lcg() * 0.4f;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::rotate(model, yRot, glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(scale));

        modelMatrices[i] = model;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

    // to do
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);

    for (int col = 0; col < 4; col++) {
        GLuint loc = 3 + col;
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE,
            sizeof(glm::mat4),
            (void*)(col * sizeof(glm::vec4)));
        glVertexAttribDivisor(loc, 1);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
