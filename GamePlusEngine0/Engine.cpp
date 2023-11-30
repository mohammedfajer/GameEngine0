#include "Engine.h"
#include "Logger.h"



#include <string>




namespace IceEngine {

	
	Engine& Engine::GetInstance() {
		static Engine instance;
		return instance;
	}

	void Engine::Start()
	{
		Logger::GetInstance().Log("Starting Engine");

		InitSDL();
		CreateWindow("Game", SCREEN_WIDTH, SCREEN_HEIGHT);
		InitGLContext();
		InitGLEW();

		// Check OpenGL Version
		const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		if (version)
			Logger::GetInstance().Log("OpenGL Version " + std::string(version));
	}

	void Engine::Update()
	{
		
	}



	void Engine::Shutdown()
	{
		Logger::GetInstance().Log("Shutting Down Engine");
	}


	void Engine::InitSDL()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			Logger::GetInstance().Log("SDL Initialization Failed : " + std::string(SDL_GetError()), LogLevel::ERROR);
			HaltQuit();
		}
	}

	void Engine::CreateWindow(const char* title, int width, int height)
	{
		m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);

		if (!m_window) {
			Logger::GetInstance().Log("Window Creation Failed : " + std::string(SDL_GetError()), LogLevel::ERROR);
			HaltQuit();
		}
	}

	void Engine::InitGLContext()
	{
		// Create an OpenGL Context
		m_glContext = SDL_GL_CreateContext(m_window);

	}

	void Engine::InitGLEW()
	{
		// Initialize GLEW
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK) {
			Logger::GetInstance().Log("GLEW Initialization Failed : " + std::string(SDL_GetError()), LogLevel::ERROR);
			HaltQuit();
		}
	}

}







