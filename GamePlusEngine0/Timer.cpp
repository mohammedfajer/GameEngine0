#include "Timer.h"
#include <SDL.h>


#define FPS 60

namespace IceEngine  {

	Timer& Timer::Instance() {
		static Timer instance;	
		return instance;
	}
	
 //
	//float Timer::Tick() {
	//	Uint32 current_time = SDL_GetTicks();
	//	Uint32 elapsed_time = (current_time - last_frame_time) / 1000.0f;
	//	last_frame_time = current_time;
	//	if (elapsed_time < static_cast<Uint32>(1000.0f / FPS)) {
	//		SDL_Delay(static_cast<Uint32>((1000.0f / FPS)) - elapsed_time);
	//	}
	//	return static_cast<float>(elapsed_time); // return in seconds
	//}
	//float Timer::Tick() {
	//	Uint32 current_time = SDL_GetTicks();
	//	Uint32 elapsed_time = current_time - last_frame_time;
	//	last_frame_time = current_time;

	//	// Convert elapsed_time to seconds
	//	float elapsed_seconds = static_cast<float>(elapsed_time) / 1000.0f;

	//	if (elapsed_time < static_cast<Uint32>(1000.0f / FPS)) {
	//		SDL_Delay(static_cast<Uint32>((1000.0f / FPS)) - elapsed_time);
	//	}

	//	return elapsed_seconds; // return in seconds
	//}

	float Timer::Tick() {
		Uint32 current_time = SDL_GetTicks();
		Uint32 elapsed_time = current_time - last_frame_time;
		last_frame_time = current_time;

		float elapsed_seconds = static_cast<float>(elapsed_time) / 1000.0f;

		// Calculate the time it should take for one frame
		float frame_time = 1000.0f / FPS;

		if (elapsed_seconds < static_cast<Uint32>(frame_time)) {
			Uint32 delay_time = static_cast<Uint32>(frame_time - elapsed_seconds);

			// Clamp the delay to a non-negative value
			if (delay_time > 0) {
				SDL_Delay(delay_time);
			}
		}

		return elapsed_seconds; // return in seconds
	}

	
	
}
