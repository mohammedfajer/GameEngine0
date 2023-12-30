#pragma once

#include "config.h"
#include "Logger.h"

#include <SDL.h>

START_NAMESPACE

/*
	Timer t(3.0f); // 3 seconds

	if(t.isExpired()) {
		// 3 seconds finished
	}
*/

struct Timer {
	Uint32 startTime;
	Uint32 duration;
	Timer() = default;
	Timer(Uint32 duration);

	void set_time(Uint32 seconds);


	void reset();
	bool isExpired() const;
};

struct Clock {
	uint32_t last_tick_time = 0;
	uint32_t delta = 0;

	void tick();
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

		if (frame_time >= 1000) {
			fps = static_cast<float>(frame_count) / (frame_time / 1000.0f);

			frame_count = 0;
			last_frame_time = current_ticks;

			Logger::Instance().Log(LogLevel::INFO, "FPS = %", fps);
		}
	}
};


END_NAMESPACE