#pragma once
#include <GL/glew.h>
#include <iostream>

#define SCREEN_WIDTH 1834
#define SCREEN_HEIGHT 760

namespace IceEngine
{
	/*inline float convert_x_from_world_to_ndc(float value)
	{
		return 2 * ((value - SCREEN_WIDTH) / SCREEN_WIDTH) + 1;
	}

	inline float convert_y_from_world_to_ndc(float value)
	{
		return 2 * ((value - SCREEN_HEIGHT) / SCREEN_HEIGHT) + 1;
	}*/

	inline float convert_x_from_world_to_ndc(float value)
	{
		return (2.0f * value / SCREEN_WIDTH) - 1.0f;
	}

	inline float convert_y_from_world_to_ndc(float value)
	{
		return 1.0f - (2.0f * value / SCREEN_HEIGHT);
	}

	// Function to check shader compilation errors
	static void checkShaderCompileError(GLuint shader) {
		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLint logLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
			char *log = new char[logLength];
			glGetShaderInfoLog(shader, logLength, NULL, log);
			std::cerr << "Shader compilation error:\n" << log << std::endl;
			delete[] log;
		}
	}

	// Function to check program linking errors
	static void checkProgramLinkError(GLuint program) {
		GLint success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			GLint logLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			char *log = new char[logLength];
			glGetProgramInfoLog(program, logLength, NULL, log);
			std::cerr << "Program linking error:\n" << log << std::endl;
			delete[] log;
		}
	}
}