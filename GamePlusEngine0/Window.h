#pragma once


#include <SDL.h>
#include <GL/glew.h>

namespace IceEngine {
	class Window
	{
	public:
		Window(const char* title, int width, int height);
		void Update();

		SDL_Window* GetWindow() const { return m_window; }

	private:
		SDL_Window* m_window;
		SDL_GLContext m_glContext;

		void InitSDL();
		void CreateWindow(const char* title, int width, int height);
		void InitGLContext();
		void InitGLEW();
		
	
	};
}
