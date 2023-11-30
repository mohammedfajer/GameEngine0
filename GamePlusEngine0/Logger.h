#pragma once

// Engine.h
#pragma once
#include <string>

namespace IceEngine {
	enum class LogLevel {
		SUCCESS,
		INFO,
		ERROR,
		RESET
	};

	class Logger {
	public:
		static Logger& GetInstance();
		static void Log(std::string message, LogLevel level = LogLevel::INFO);
	
		// Disable copy and move operations
		Logger(Logger const&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator=(Logger const&) = delete;
		Logger& operator=(Logger&&) = delete;
	private:
		Logger() = default;
		~Logger() = default;
	};
} 