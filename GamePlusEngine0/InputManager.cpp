#include "InputManager.h"
#include "Logger.h"
#include "Engine.h"

// Dear IMGUI
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

namespace IceEngine 
{
	InputManager& InputManager::Instance() 
	{
		static InputManager instance;
		return instance;
	}
	
	InputManager::InputManager() 
	{
		m_isKeyRepeat = false;
		m_signalQuit = false;
		if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) < 0) 
		{
			Logger::Instance().Log("Failed to initialize input subsystem", LogLevel::ERROR);
		}
	}

	InputManager::~InputManager() 
	{
		SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
	}

	void InputManager::Update() 
	{
		// clear the set of keys pressed in the previous frame
		m_keysPressed.clear();

		SDL_Event event;
		while(SDL_PollEvent(&event)) 
		{

			ImGui_ImplSDL2_ProcessEvent(&event);

			HandleEvent(event);
		}
	}

	void InputManager::HandleEvent(SDL_Event &event) 
	{
		switch(event.type) {

		case SDL_QUIT:
			m_signalQuit = true;
			break;
			
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			HandleKeyboardEvent(event);
			break;

		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			HandleMouseEvent(event);
			break;

		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
			HandleGamepadEvent(event);
			break;

		default:
			break;
		}
	}

	void InputManager::HandleKeyboardEvent(SDL_Event &event) 
	{
		if (event.key.repeat == 0)
		{
			m_keyboardState[event.key.keysym.scancode] = 1;
			m_keysPressed.insert(event.key.keysym.scancode);
			m_isKeyRepeat = false;
		}
		else
		{
			m_isKeyRepeat = true;
		}

		// If it's a key release event, reset the key state
		if (event.type == SDL_KEYUP)
		{
			m_keyboardState[event.key.keysym.scancode] = 0;
			m_keysPressed.erase(event.key.keysym.scancode);
		}
	}

	void InputManager::HandleMouseEvent(SDL_Event &event) 
	{
		if(event.type == SDL_MOUSEMOTION) 
		{
			m_mousePosition = glm::vec2(event.motion.x, event.motion.y);
		} 
		else 
		{
			m_mouseState [event.button.button] = event.button.state;
		}
	}

	void InputManager::HandleGamepadEvent(SDL_Event &event)
	{
		m_gamepadState[event.cbutton.button] = event.cbutton.state;
	}

	bool InputManager::IsKeyDown(SDL_Scancode key) const 
	{
		return m_keyboardState[key] == 1;
	}

	bool InputManager::IsKeyUp(SDL_Scancode key) const 
	{
		return m_keyboardState[key] == 0;
	}

	bool InputManager::IsMouseButtonDown(Uint8 button) const 
	{
		return m_mouseState[button] == SDL_PRESSED;
	}

	bool InputManager::IsMouseButtonUp(Uint8 button) const 
	{
		return m_mouseState[button] == SDL_RELEASED;
	}

	bool InputManager::IsGamepadButtonDown(SDL_GameControllerButton button) const 
	{
		return m_gamepadState[button] == SDL_PRESSED;
	}

	bool InputManager::IsGamepadButtonUp(SDL_GameControllerButton button) const 
	{
		return m_gamepadState[button] == SDL_RELEASED;
	}
	bool InputManager::IsKeyPressed(SDL_Scancode key) const
	{
	
		// Check if the key is in the set of keys pressed in the current frame
		return m_keysPressed.count(key) > 0 && m_isKeyRepeat == false;
	}

	bool InputManager::IsKeyReleased(SDL_Scancode key) const
	{
		// Check if the key is released in the current frame
		// Check if the key was released in the current frame and was not immediately pressed again due to key repeat
		return (m_keyboardState[key] == 0) && (m_keysPressed.count(key) == 0) && !m_isKeyRepeat;
	}
}

