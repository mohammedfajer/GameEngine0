
#include "time.h"


START_NAMESPACE


// Timer
Timer::Timer(Uint32 duration) 
	: duration(static_cast<Uint32>(duration * 1000.0f)) { 
	
	reset(); 
}

void Timer::set_time(Uint32 seconds) {
	duration = seconds * 1000.0f;
	reset();
}

void Timer::reset() {
	startTime = SDL_GetTicks();
}

bool Timer::isExpired() const {
	Uint32 currentTime = SDL_GetTicks();
	return (currentTime - startTime) >= duration;
}

// Clock 
void Clock::tick() {
	uint32_t tick_time = SDL_GetTicks();
	delta = tick_time - last_tick_time;
	last_tick_time = tick_time;
}



END_NAMESPACE