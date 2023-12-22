#include "FileIO.h"

namespace IceEngine {
	const char *LoadTextFile(const char *filePath) {
		std::ifstream file(filePath);
        
		if (!file.is_open()) {
			Logger::Instance().Log("Error opening file: " + std::string(filePath), LogLevel::ERROR);
			return nullptr;
		}
        
		// Read the entire file into a stringstream
		std::stringstream stream;
		stream << file.rdbuf();
        
		// Allocate memory for a C-style string and copy the contents
		std::string shaderSource = stream.str();
		char* shaderSourceCStr = new char[shaderSource.length() + 1];
        
		// Use strcpy_s instead of strcpy
		strcpy_s(shaderSourceCStr, shaderSource.length() + 1, shaderSource.c_str());
        
		file.close();
        
		return shaderSourceCStr;
	}
}
