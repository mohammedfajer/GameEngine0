#include "Window.h"

#include "Logger.h"

#include "Color.h"
#include "InputManager.h"
#include "Engine.h"


// Dear IMGUI
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

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


		// Init DearIMGUI 

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF("./data/fonts/arial.ttf", 18.0f);

		// Setup Dear ImGui style
		//ImGui::StyleColorsDark();
		ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer 
		ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
		
		ImGui_ImplOpenGL3_Init(m_glsl_version.c_str());
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
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0) {
			Logger::Instance().Log("SDL Initialization Failed : " + std::string(SDL_GetError()), LogLevel::ERROR);
			exit(-1);
		}


		// Decide GL+GLSL versions
	#if defined(IMGUI_IMPL_OPENGL_ES2)
			// GL ES 2.0 + GLSL 100
			const char* glsl_version = "#version 100";
			m_glsl_version = "#version 100";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	#elif defined(__APPLE__)
			// GL 3.2 Core + GLSL 150
		
			m_glsl_version = "#version 150";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	#else
			// GL 3.0 + GLSL 130
			
			m_glsl_version = "#version 130";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	#endif


		// From 2.0.18: Enable native IME.
	#ifdef SDL_HINT_IME_SHOW_UI
		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
	#endif


		
	}

	void Window::CreateWindow(const char* title, int width, int height)
	{

		// Create window with graphics context
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

		if (!m_window) {
			Logger::Instance().Log("Window Creation Failed : " + std::string(SDL_GetError()), LogLevel::ERROR);
			exit(-1);
		}
	}

	void Window::InitGLContext()
	{
		// Create an OpenGL Context
		m_glContext = SDL_GL_CreateContext(m_window);

		SDL_GL_MakeCurrent(m_window, m_glContext);
		SDL_GL_SetSwapInterval(1); // Enable vsync

		//glDisable(GL_STENCIL_TEST);
		
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