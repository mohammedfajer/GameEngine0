#include "Engine.h"
#include "Logger.h"
#include <string>

#include "Timer.h"
#include "InputManager.h"
#include "SceneManager.h"

#include "SpritesheetScene.h"

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

		m_runMode = RunMode::GameMode;

		m_window = new Window("Game", SCREEN_WIDTH, SCREEN_HEIGHT);

		// Starting Scene the game ...


		Scene* spriteSheetScene = new TopDownShooter::SpriteSheetScene();
		SceneManager::Instance().AddScene("SpriteSheetScene", spriteSheetScene);

		// Set the active Scene
		SceneManager::Instance().SetActiveScene("SpriteSheetScene");

		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_running = true;
	}

	void Engine::Update()
	{
		

		// Toggle Play mode or editor mode

		if (InputManager::Instance().IsKeyDown(SDL_SCANCODE_E))
		{
			m_runMode = (m_runMode == RunMode::GameMode) ? RunMode::EditorMode : RunMode::GameMode;
			if (m_runMode == RunMode::EditorMode)
			{
				Logger::Instance().Log("Editor Mode", LogLevel::SUCCESS);
			}
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

		/*if (m_runMode == RunMode::GameMode) {


		}*/

		
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







