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

// grass.cpp
float Grass::RandomFloat(float min, float max)
{
    float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return min + t * (max - min);
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

    noise.Load("simplex_height.png", 30.0f, 2624.0f, 1756.0f);

    std::vector<glm::mat4> modelMatrices;
    modelMatrices.reserve(amount);

    while (modelMatrices.size() < amount)
    {
        float x = RandomFloat(-(2624.0f) * 0.5f, (2624.0f) * 0.5f);
        float z = RandomFloat(-(1756.0f) * 0.5f, (1756.0f) * 0.5f);

        float y = noise.GetHeightWorld(x, z) + 10.0f;
        glm::vec3 normal = noise.GetNormalWorld(x, z, 1.0f);

        // Skip steep places
        if (normal.y < 0.8f)
            continue;

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(x, y, z));

        float yaw = RandomFloat(0.0f, 6.2831853f);
        model = glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));

        float scale = RandomFloat(0.8f, 1.2f);
        model = glm::scale(model, glm::vec3(scale));

        modelMatrices.push_back(model);
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
