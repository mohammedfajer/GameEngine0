#include "Engine.h"
#include "Logger.h"
#include <string>

#include "Timer.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "GameScene.h"
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

		// Set the active Scene
		SceneManager::Instance().SetActiveScene("Game");

		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_running = true;
		
	}

	void Engine::Update()
	{
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

		SDL_GL_SwapWindow(m_window->GetWindow());
	}


	Engine::~Engine()
	{
		if(m_window)
			delete m_window;
	}
}







