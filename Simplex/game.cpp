#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "resource_manager.h"

Game::Game(uint32_t width, uint32_t height)
	: State(GAME_ACTIVE), Width(width), Height(height), Keys()
{
}

void Game::Init() 
{
	camera = std::make_unique<Camera>(glm::vec3(0.f, 2.f, 6.f));
	terrain = std::make_unique<Terrain>();
	// load shaders
	ResourceManager::LoadShader("simplex.vs", "simplex.fs", "simplex.tcs", "simplex.tes", nullptr, "terrain");
	// configure shaders
	
	// load textures
	terrain->LoadHeightmap("grass.png");
	terrain->Build();
}

void Game::ProcessInput(GLFWwindow* window, float deltaTime) const
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, deltaTime);
	
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera->ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera->ProcessKeyboard(UP, deltaTime);
}

void Game::Update(float dt)
{
}

void Game::Render() const
{
	if (State == GAME_ACTIVE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glm::mat4 view = camera->GetViewMatrix();
		auto& ts = ResourceManager::GetShader("terrain").Use();
		glm::mat4 tModel = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)Width / (float)Height, 0.1f, 100000.0f);
		terrain->DrawTerrain(ts, tModel, view, projection);
	}
}

void Game::OnMouseDelta(float x, float y) const
{
	camera->ProcessMouseMovement(x, y);
}
