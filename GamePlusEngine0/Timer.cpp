#include "Timer.h"
#include <SDL.h>

#define FPS 60.0f

namespace IceEngine 
{
	Timer& Timer::Instance() 
	{
		static Timer instance;	
		return instance;
	}
	
	float Timer::Tick()
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime);

		// Ensure duration is not zero to prevent division by zero
		float deltaTime = (duration.count() > 0) ? (duration.count() / 1000.0f) : 0.0001f;

		lastFrameTime = currentTime;

		// Calculate the time to sleep to achieve the desired FPS
		long long sleepTime = static_cast<long long>(1000.0f / FPS) - duration.count();
		if (sleepTime > 0) {
			SDL_Delay((int32_t) sleepTime);
		}

		return deltaTime;
	}
}
