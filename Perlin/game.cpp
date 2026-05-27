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
	ResourceManager::LoadShader("perlin.vs", "perlin.fs", "perlin.tcs", "perlin.tes", nullptr, "terrain");
	ResourceManager::LoadShader("cubemap.vs", "cubemap.fs", nullptr, nullptr, nullptr, "skybox");
	ResourceManager::LoadShader("grass.vs", "grass.fs", nullptr, nullptr, nullptr, "grass");
	// configure shaders
	ResourceManager::GetShader("grass").setInteger("texture_diffuse1", 0);
	// load textures
	terrain->LoadHeightmap("grass.png");
	terrain->Build();

	ResourceManager::LoadTexture("grassa.png", true, "grassTex");

	// grass
	grass = std::make_unique<Grass>(ResourceManager::GetShader("grass"), 50.0f, 300.0f, 500.0f);

	skybox = std::make_unique<Cubemap>(ResourceManager::GetShader("skybox"));
	skybox->Generate(this->faces);
	
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
		glm::mat4 view = camera->GetViewMatrix();
		auto& ts = ResourceManager::GetShader("terrain").Use();
		glm::mat4 tModel = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)Width / (float)Height, 0.1f, 100000.0f);
		terrain->DrawTerrain(ts, tModel, view, projection);
		
		//grass
		glDisable(GL_CULL_FACE);
		grass->Draw(ResourceManager::GetTexture("grassTex"), view, projection);
		glEnable(GL_CULL_FACE);
		// skybox
		ts.SetVec3f("CameraWoldPos", camera->Position);
		glm::mat4 skyView = glm::mat4(glm::mat3(view));
		skybox->Render(skyView, projection);
	}
}

void Game::OnMouseDelta(float x, float y) const
{
	camera->ProcessMouseMovement(x, y);
}
