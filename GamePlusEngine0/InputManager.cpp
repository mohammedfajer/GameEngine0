#include "InputManager.h"

#include "Logger.h"

namespace IceEngine {
	InputManager::InputManager()
	{
		if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) < 0) {
			// Handle initialization error
		}
	}

	InputManager::~InputManager()
	{
		SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
	}

	InputManager& InputManager::Instance()
	{
		// TODO: insert return statement here
		static InputManager instance;
		return instance;
	}

}

