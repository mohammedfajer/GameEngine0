#include "InputManager.h"

#include "Logger.h"

namespace IceEngine {

	InputManager& InputManager::Instance() {
		static InputManager instance;
		return instance;
	}
	
	InputManager::InputManager() {
		m_isKeyRepeat = false;
		if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) < 0) {
			Logger::Instance().Log("Failed to initialize input subsystem", LogLevel::ERROR);
		}
	}

	InputManager::~InputManager() {
		SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
	}

	void InputManager::Update() {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			HandleEvent(event);
		}
	}

	void InputManager::HandleEvent(SDL_Event &event) {
		switch(event.type) {

		case SDL_QUIT:
			exit(0);
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

	void InputManager::HandleKeyboardEvent(SDL_Event &event) {
		if(event.key.repeat == 0) {
			m_keyboardState[event.key.keysym.scancode] = (event.type == SDL_KEYDOWN) ? 1: 0;
 		} else {
			m_isKeyRepeat = true;
		}
	}

	void InputManager::HandleMouseEvent(SDL_Event &event) {
		if(event.type == SDL_MOUSEMOTION) {
			m_mousePosition = glm::vec2(event.motion.x, event.motion.y);
		} else {
			m_mouseState [event.button.button] = event.button.state;
		}
	}

	void InputManager::HandleGamepadEvent(SDL_Event &event) {
		m_gamepadState[event.cbutton.button] = event.cbutton.state;
	}

	
	bool InputManager::IsKeyDown(SDL_Scancode key) const {
		return m_keyboardState[key] == 1;
	}

	bool InputManager::IsKeyUp(SDL_Scancode key) const {
		return m_keyboardState[key] == 0;
	}

	bool InputManager::IsMouseButtonDown(Uint8 button) const {
		return m_mouseState[button] == SDL_PRESSED;
	}

	bool InputManager::IsMouseButtonUp(Uint8 button) const {
		return m_mouseState[button] == SDL_RELEASED;
	}

	bool InputManager::IsGamepadButtonDown(SDL_GameControllerButton button) const {
		return m_gamepadState[button] == SDL_PRESSED;
	}

	bool InputManager::IsGamepadButtonUp(SDL_GameControllerButton button) const {
		return m_gamepadState[button] == SDL_RELEASED;
	}

}

