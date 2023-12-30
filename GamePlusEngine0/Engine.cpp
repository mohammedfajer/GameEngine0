#include "Engine.h"
#include "Logger.h"
#include <string>


#include "InputManager.h"
#include "SceneManager.h"

#include "SpritesheetScene.h"

#include "Color.h"
#include "Menu.h"
#include "FontSystem.h"

// Dear IMGUI
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"


#include "core.h"

namespace IceEngine 
{
	Engine &Engine::Instance() 
	{
		static Engine instance;
		return instance;
	}

	void Engine::Start()
	{
		//Timer::Instance().StartRecordTime();

		Logger::Instance().Log("Starting Engine");



	

		m_window = new Window("Game", SCREEN_WIDTH, SCREEN_HEIGHT);

		// Starting Scene the game ...

		Scene* spriteSheetScene = new TopDownShooter::SpriteSheetScene();
		
		SceneManager::Instance().AddScene("SpriteSheetScene", spriteSheetScene);

		Scene *menuScene = new TopDownShooter::MenuScene();
		SceneManager::Instance().AddScene("MenuScene", menuScene);

		// Set the active Scene
		SceneManager::Instance().SetActiveScene("MenuScene");

		// Enable blending
		glEnable(GL_BLEND);		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_running = true;
	}

	void Engine::Update()
	{
		// Toggle Play mode or editor mode

		if (InputManager::Instance().IsKeyPressed(SDL_SCANCODE_E))
		{
			Core::app_mode = (Core::app_mode == App_Mode::Game) ? App_Mode::Editor : App_Mode::Game;
		}
	}

	void Engine::Shutdown()
	{
		Logger::Instance().Log("Shutting Down Engine");
		Logger::Instance().Log("", LogLevel::RESET);
	}

	void Engine::Run() 
	{
		float dt = Core::time_info.dt;

		InputManager::Instance().Update();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

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
		ImGuiIO &io = ImGui::GetIO(); (void)io;
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

		Color::SetClearColor({ 66, 135, 245 , 255 });
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Rendering should go here
		SceneManager::Instance().RenderCurrentScene();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		SDL_GL_SwapWindow(m_window->GetWindow());
	}
	
	Engine::~Engine()
	{
		// Cleanup
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		if(m_window)
			delete m_window;
	}
}







