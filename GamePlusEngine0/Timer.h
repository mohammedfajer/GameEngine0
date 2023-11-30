#pragma once
#include <chrono>

namespace IceEngine {
	class Timer {
	public:
		static Timer& Instance();
		float Tick();
	private:
		Timer() : deltaTime(0.0f) { lastFrameTime = std::chrono::high_resolution_clock::now(); }
		float deltaTime;
		std::chrono::high_resolution_clock::time_point lastFrameTime;
	};
}
