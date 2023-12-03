#include "Engine.h"
#include "Logger.h"
#include <string>

#include "Timer.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "SpritesheetScene.h"
#include "SpriteBatchScene.h"
#include "Color.h"

namespace IceEngine 
{
	Engine &Engine::Instance() 
	{
		static Engine instance;
		return instance;
	}

	void Engine::Start()
	{
		Logger::Instance().Log("Starting Engine");

		m_window = new Window("Game", SCREEN_WIDTH, SCREEN_HEIGHT);

		// Starting Scene the game ...
		Scene* gameScene = new TopDownShooter::GameScene();
		SceneManager::Instance().AddScene("Game", gameScene);

		Scene* spriteBatchScene = new TopDownShooter::SpriteBatchScene();
		SceneManager::Instance().AddScene("SpriteBatchScene", spriteBatchScene);

		Scene* spriteSheetScene = new TopDownShooter::SpriteSheetScene();
		SceneManager::Instance().AddScene("SpriteSheetScene", spriteSheetScene);

		// Set the active Scene
		SceneManager::Instance().SetActiveScene("SpriteBatchScene");

		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_running = true;
	}

	void Engine::Update()
	{
		if (InputManager::Instance().IsKeyDown(SDL_SCANCODE_1))
		{
			SceneManager::Instance().SetActiveScene("Game");
		}

		if (InputManager::Instance().IsKeyDown(SDL_SCANCODE_2))
		{
			SceneManager::Instance().SetActiveScene("SpriteSheetScene");
		}

		if (InputManager::Instance().IsKeyDown(SDL_SCANCODE_3))
		{
			SceneManager::Instance().SetActiveScene("SpriteBatchScene");
		}
	}

	void Engine::Shutdown()
	{
		Logger::Instance().Log("Shutting Down Engine");
		Logger::Instance().Log("", LogLevel::RESET);
	}

	void Engine::Run() 
	{
		float dt = Timer::Instance().Tick();

		InputManager::Instance().Update();

		m_window->Update();
		
		SceneManager::Instance().UpdateCurrentScene(dt);

		Update();

		AfterUpdate();
		
		Render();
	}

	void Engine::AfterUpdate()
	{
	}

	void Engine::Render()
	{
		Color::SetClearColor({ 66, 135, 245 , 255 });
		glClear(GL_COLOR_BUFFER_BIT);

		// Rendering should go here

		SceneManager::Instance().RenderCurrentScene();

		SDL_GL_SwapWindow(m_window->GetWindow());
	}


	Engine::~Engine()
	{
		if(m_window)
			delete m_window;
	}
}







