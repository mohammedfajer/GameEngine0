#pragma once

#include <chrono>


namespace IceEngine {

	class Timer {
	public:
		static Timer& GetInstance();

		void Update();

		float GetDeltaTime() const;

	private:
		Timer() : deltaTime(0.0f) {}


		float deltaTime;
		std::chrono::high_resolution_clock::time_point lastFrameTime;
	};

}
