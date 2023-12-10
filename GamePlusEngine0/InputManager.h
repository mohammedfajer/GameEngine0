#pragma once

#include <SDL.h>
#include <glm/glm.hpp>
#include "Logger.h"
#include <unordered_set>

namespace IceEngine {
	class InputManager
	{
	public:
		static InputManager& Instance();
		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;
	public:
		void Update();
	public:
		glm::vec2 GetMousePosition() const { return m_mousePosition; }

		bool IsKeyDown(SDL_Scancode key) const;
		bool IsKeyUp(SDL_Scancode key) const;

		bool IsMouseButtonDown(Uint8 button) const;
		bool IsMouseButtonUp(Uint8 button) const;

		bool IsGamepadButtonDown(SDL_GameControllerButton button) const;
		bool IsGamepadButtonUp(SDL_GameControllerButton button) const;

		bool IsKeyPressed(SDL_Scancode key) const;
		bool IsKeyReleased(SDL_Scancode key) const;
	
	private:
		void HandleEvent(SDL_Event &event);
		void HandleKeyboardEvent(SDL_Event &event);
		void HandleMouseEvent(SDL_Event &event);
		void HandleGamepadEvent(SDL_Event& event);
	public:
		bool m_signalQuit;
		bool m_isKeyRepeat;
	private:
		Uint8 m_keyboardState	[SDL_NUM_SCANCODES];
		Uint32 m_mouseState		[8];
		glm::vec2 m_mousePosition;
		
		Uint8 m_gamepadState	[SDL_CONTROLLER_BUTTON_MAX];
		
		// Keep track of keys that were pressed in the current frame
		std::unordered_set<SDL_Scancode> m_keysPressed;
	private:
		InputManager();
		~InputManager();
	};
}


