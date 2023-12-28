#pragma once

#include "SDL.h"
#include "Logger.h"

namespace IceEngine {
	class Timer {
	public:
		static Timer& Instance();
		double Tick();
	private:
		Timer() = default;
		
		uint64_t last_frame_time;
	};

	struct FPS_Counter {
		uint32_t frame_count;
		uint32_t last_frame_time;

		float fps;

		FPS_Counter() {
			frame_count = 0;
			last_frame_time = 0;
			fps = 0.0f;
		}

		void update() {
			uint32_t current_ticks = SDL_GetTicks();
			frame_count++;

			uint32_t frame_time = current_ticks - last_frame_time;

			if(frame_time >= 1000) {
				fps = static_cast<float>(frame_count) / (frame_time / 1000.0f);

				frame_count = 0;
				last_frame_time = current_ticks;

				Logger::Instance().Log(LogLevel::INFO, "FPS = %", fps);
			}
		}
	};

	/*
		FPS_Counter fps_counter;

		fps_counter.update();
	 */
	
}
