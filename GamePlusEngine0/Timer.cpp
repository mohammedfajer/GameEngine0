#include "Timer.h"
#include <SDL.h>


#define FPS 60.0f

namespace IceEngine  {

	Timer& Timer::Instance() {
		static Timer instance;	
		return instance;
	}
	
 
	double Timer::Tick() {

		uint64_t current_time = SDL_GetPerformanceCounter();
		double duration = static_cast<double>(current_time - last_frame_time);

		double elapsed_time_milliseconds = duration / static_cast<double>(SDL_GetPerformanceFrequency());
		last_frame_time = current_time;

		long long sleep_time = static_cast<long long>((1000.0f / FPS) - elapsed_time_milliseconds);

		if (sleep_time > 0) {
			SDL_Delay(static_cast<Uint32>(sleep_time));
		}

		return elapsed_time_milliseconds / 1000.0f;
	}
	

	
	
}
