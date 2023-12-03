#pragma once

// Engine.h
#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <stdarg.h>
#include <iostream>


namespace IceEngine {
	enum class LogLevel {
		SUCCESS,
		INFO,
		ERROR,
		RESET
	};

	class Logger {
	public:
		static Logger& Instance();
		static void Log(std::string message, LogLevel level = LogLevel::INFO);

		template <typename... Args>
		static void Log(LogLevel level, const char* format, Args... args) {
			std::stringstream ss;
			switch (level) {
			case LogLevel::SUCCESS:
				ss << "\x1b[32m[SUCCESS] ";
				break;
			case LogLevel::INFO:
				ss << "\x1b[94m[INFO] ";
				break;
			case LogLevel::ERROR:
				ss << "\x1b[31m[ERROR] ";
				break;
			case LogLevel::RESET:
				std::cerr << "\x1b[0m" << std::endl;
				return;
			}

			LogInternal(ss, format, args...);
		}
	
		// Disable copy and move operations
		Logger(Logger const&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator=(Logger const&) = delete;
		Logger& operator=(Logger&&) = delete;
	private:
		template <typename T, typename... Args>
		static void LogInternal(std::stringstream& ss, const char* format, T value, Args... args) {
			while (*format) {
				if (*format == '%' && *(format + 1) == '%') {
					format += 2;
				}
				else if (*format == '%') {
					ss << value;
					LogInternal(ss, format + 1, args...);
					return;
				}
				else {
					ss << *format;
				}
				++format;
			}
			std::cout << ss.str() << std::endl;
		}

		static void LogInternal(std::stringstream& ss, const char* format) {
			while (*format) {
				if (*format == '%' && *(format + 1) == '%') {
					format += 2;
				}
				else {
					ss << *format;
				}
				++format;
			}
			std::cout << ss.str() << std::endl;
		}


		Logger() = default;
		~Logger() = default;
	};
} 