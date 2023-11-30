#include "Timer.h"

namespace IceEngine {
	Timer& Timer::GetInstance() {
		static Timer instance;
		return instance;
	}
	void Timer::Update() {
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime);
		deltaTime = duration.count() / 1000.0f;
		lastFrameTime = currentTime;
	}
	float Timer::GetDeltaTime() const {
		return deltaTime; 
	}
}
