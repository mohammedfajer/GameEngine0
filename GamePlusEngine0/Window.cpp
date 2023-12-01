#include "Window.h"

#include "Logger.h"

#include "Color.h"
#include "InputManager.h"
#include "Engine.h"

namespace IceEngine {


	Window::Window(const char *title, int width, int height) 
	{
		InitSDL();
		CreateWindow(title, width, height);
		InitGLContext();
		InitGLEW();

		// Check OpenGL Version
		const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		if (version)
			Logger::Instance().Log("OpenGL Version " + std::string(version));
	}

	void Window::Update()
	{
		if (InputManager::Instance().m_signalQuit) 
		{
			IceEngine::Engine::Instance().m_running = false;
		}

		if (InputManager::Instance().IsKeyDown(SDL_SCANCODE_ESCAPE))
		{
			IceEngine::Engine::Instance().m_running = false;
		}
	}
	
	void Window::InitSDL()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			Logger::Instance().Log("SDL Initialization Failed : " + std::string(SDL_GetError()), LogLevel::ERROR);
			exit(-1);
		}
	}

	void Window::CreateWindow(const char* title, int width, int height)
	{

		m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);

		if (!m_window) {
			Logger::Instance().Log("Window Creation Failed : " + std::string(SDL_GetError()), LogLevel::ERROR);
			exit(-1);
		}
	}

	void Window::InitGLContext()
	{
		// Create an OpenGL Context
		m_glContext = SDL_GL_CreateContext(m_window);
	}

	void Window::InitGLEW()
	{
		// Initialize GLEW
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK) {
			Logger::Instance().Log("GLEW Initialization Failed : " + std::string(SDL_GetError()), LogLevel::ERROR);
			exit(-1);
		}
	}
}