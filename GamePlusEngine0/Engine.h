#pragma once

#include <SDL.h>
#include <GL/glew.h>

#include "Window.h"


#include "Defines.h"

namespace IceEngine {

	class Engine {
	public:
		static Engine& Instance();
		void Start();
		void Update();
		void Shutdown();
		void Run();
		void AfterUpdate();
		void Render();

		// Disable copy and move operations
		Engine(Engine const&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine const&) = delete;
		Engine& operator=(Engine&&) = delete;
	public:
		SDL_Window* GetWindow() const { return m_window->GetWindow(); } 
		bool m_running = false;
	private:
		Engine() = default;
		~Engine();
		Window *m_window;
		
	};
}

