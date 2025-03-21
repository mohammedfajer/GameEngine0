// Logger.cpp

#include "Logger.h"
#include <iostream>

namespace IceEngine {
	Logger &Logger::Instance() {
		static Logger instance;
		return instance;
	}

	void Logger::Log(std::string message, LogLevel level) {
		switch (level) {
		case LogLevel::SUCCESS:
			std::cout << "\x1b[32m" << "[SUCCESS] " << message << std::endl;
			break;
		case LogLevel::INFO:
			std::cout << "\x1b[94m" << "[INFO] " << message << std::endl;
			break;
		case LogLevel::ERROR:
			std::cerr << "\x1b[31m" << "[ERROR] " << message << std::endl;
			break;

		case LogLevel::RESET:
			std::cerr << "\x1b[0m" << std::endl;
			break;
		}
	}
} 