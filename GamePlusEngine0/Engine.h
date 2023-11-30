#pragma once

#include <SDL.h>
#include <GL/glew.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

namespace IceEngine {
	

	

	class Engine {
	public:
		static Engine& GetInstance();
		

		void Start();
		void Update();
		void Shutdown();


		// Disable copy and move operations
		Engine(Engine const&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine const&) = delete;
		Engine& operator=(Engine&&) = delete;
		

		SDL_Window* GetWindow() const { return m_window; }

	
	private:
		Engine() = default;
		~Engine() = default;

		SDL_Window *m_window;
		SDL_GLContext m_glContext;

		void InitSDL();
		void CreateWindow(const char* title, int width, int height);
		void InitGLContext();
		void InitGLEW();

		void HaltQuit() {
			exit(-1);
		}


	};
}

