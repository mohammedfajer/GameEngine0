#include "Engine.h"
//#include "box2d/box2d.h"



// 
// float dt = Core::time_info.current_dt;

#include "core.h"


#include <stdio.h> // for sprintf

#include <SDL.h>
int main(int argc, char *argv[]) {

	// How many increments the clock goes through in one second.
	int64_t performance_counter_frequency = SDL_GetPerformanceFrequency();
	

	const float FPS = 60.0f;
	double frame_time = 1000.0f / FPS;

	IceEngine::Engine::Instance().Start();

	// Main Loop

	// Guarantee to not miss any time.
	// time window is all accounted for nothing escape the window.
	Uint64 last_counter = SDL_GetPerformanceCounter();


	while (IceEngine::Engine::Instance().m_running) {

		IceEngine::Engine::Instance().Run();


		// Timing
		// 
		// Machine Readable Wall Clock time
		Uint64 end_counter = SDL_GetPerformanceCounter();
		int64_t counter_elapsed = end_counter - last_counter;

		// Human Readable Wall Clock time

		double milliseconds_per_frame = static_cast<double>((1000.0f * static_cast<double>(counter_elapsed)) / static_cast<double>(performance_counter_frequency));

		double frames_per_second = performance_counter_frequency / (float)counter_elapsed;

		// Log to console
		char buffer[256];
		sprintf_s(buffer, "%.02fms/f, %.02ff/s\n", milliseconds_per_frame, frames_per_second);
		std::cout << buffer << std::endl;


		// Record delta time in seconds
		IceEngine::Core::time_info.dt = milliseconds_per_frame * 0.001f;


		// Frame Capping
	
		// Cap to FPS
		if (milliseconds_per_frame < frame_time) {
			SDL_Delay(static_cast<Uint32>(floor(frame_time - milliseconds_per_frame)));
		}
		
		

		last_counter = end_counter;

		

	}

	IceEngine::Engine::Instance().Shutdown();
	
	return (0);
}
