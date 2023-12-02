#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdint.h>
#include <string>

#include <GL/glew.h>

#include "Logger.h"


#include "FileIO.h"

namespace IceEngine 
{
	struct Shader 
	{
		uint32_t id = 0;

		const char* LoadShaderFromFile(const char* filePath)
		{
		
			const char* shaderSourceCStr = LoadTextFile(filePath);

			if(shaderSourceCStr)
				Logger::Instance().Log("Successfully loaded shader file " + std::string(filePath), LogLevel::SUCCESS);

			return shaderSourceCStr;
		}


		Shader(std::string vertexShaderFilePath, std::string fragmentShaderFilePath) 
		{
			auto check_error = [](uint32_t shader, std::string type) 
			{
				int success;
				char info_log[512];
				if (type == "program") 
				{
					glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
					if (!success) 
					{
						glGetShaderInfoLog(shader, 512, NULL, info_log);
						Logger::Instance().Log("Error::Shader::Compilation_Failed" + std::string(info_log), LogLevel::ERROR);
					}
				}
				else 
				{
					glGetProgramiv(shader, GL_LINK_STATUS, &success);
					if (!success) 
					{
						glGetProgramInfoLog(shader, 512, NULL, info_log);
						Logger::Instance().Log("Error::Shader::Linking_Failed" + std::string(info_log), LogLevel::ERROR);
					}
				}
			};

			const char* vs = LoadShaderFromFile(vertexShaderFilePath.c_str());
			const GLchar** vs_string = &vs;
			const char* fs = LoadShaderFromFile(fragmentShaderFilePath.c_str());
			const GLchar** fs_string = &fs;

			uint32_t vertex_shader_id, fragment_shader_id;

			// vertex shader
			vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex_shader_id, 1, vs_string, NULL);
			glCompileShader(vertex_shader_id);

			check_error(vertex_shader_id, "vertex");

			// fragment shader
			fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment_shader_id, 1, fs_string, NULL);
			glCompileShader(fragment_shader_id);

			check_error(vertex_shader_id, "fragment");

			// shader program
			id = glCreateProgram();
			glAttachShader(id, vertex_shader_id);
			glAttachShader(id, fragment_shader_id);
			glLinkProgram(id);

			check_error(id, "program");
			glDeleteShader(vertex_shader_id);
			glDeleteShader(fragment_shader_id);
		}

		void Bind() { glUseProgram(id); }

		void UnBind() { glUseProgram(0); }

		void SetBool(const std::string& name, bool value) 
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
		}

		void SetInt(const std::string& name, int value)
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
		}

		void SetFloat(const std::string& name, float value) 
		{
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
		}

		void SetMat4(const std::string& name, glm::mat4 mat) 
		{
			int matLoc = glGetUniformLocation(id, name.c_str());
			glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(mat));
		}
	};
}