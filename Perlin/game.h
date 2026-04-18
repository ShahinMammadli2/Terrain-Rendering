#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "model.h"
#include "terrain.h"
#include "cubemap.h"
#include "grass.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU
};

class Game {
public:
	GameState State;
	uint32_t  Width, Height;
	bool      Keys[1024];
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Terrain> terrain;
	std::unique_ptr<Cubemap> skybox;
	std::unique_ptr<Grass> grass;

	Texture2D grassTexture;
	const std::vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};
	// constructor
	Game(uint32_t width, uint32_t height);
	void Init();
	// game loop
	void ProcessInput(GLFWwindow* window, float deltaTime) const;
	void Update(float dt);
	void Render() const;
	void OnMouseDelta(float x, float y) const;
};

#endif // !GAME_H
