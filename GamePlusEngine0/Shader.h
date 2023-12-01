#pragma once

#include <stdint.h>
#include <string>

#include <GL/glew.h>

#include "Logger.h"

std::string vertex_shader = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec2 aTexCoord;

    out vec3 ourColor;
    out vec2 TexCoord;

    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    

	void main()
	{
		gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
        ourColor = aColor;
        TexCoord = aTexCoord;
    }
)";

std::string fragment_shader = R"(
	#version 330 core
	out vec4 FragColor;

    in vec3 ourColor;
    in vec2 TexCoord;

    uniform sampler2D ourTexture;

	void main()
	{
		FragColor = texture(ourTexture, TexCoord) ; //* vec4(ourColor, 1.0);
	}
)";

namespace IceEngine {

	struct Shader {

		uint32_t id = 0;

		Shader(std::string vertex_source, std::string fragment_source) {
			auto check_error = [](uint32_t shader, std::string type) {
				int success;
				char info_log[512];
				if (type == "program") {
					glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
					if (!success) {
						glGetShaderInfoLog(shader, 512, NULL, info_log);

						Logger::Instance().Log("Error::Shader::Compilation_Failed" + std::string(info_log), LogLevel::ERROR);
					}
				}
				else {
					glGetProgramiv(shader, GL_LINK_STATUS, &success);
					if (!success) {
						glGetProgramInfoLog(shader, 512, NULL, info_log);
						Logger::Instance().Log("Error::Shader::Linking_Failed" + std::string(info_log), LogLevel::ERROR);
					}
				}
			};

			const char* vs = vertex_shader.c_str();
			const GLchar** vs_string = &vs;
			const char* fs = fragment_shader.c_str();
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

		void use() { glUseProgram(id); }

		void set_bool(const std::string& name, bool value) {
			glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
		}

		void set_int(const std::string& name, int value) {
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
		}
		void set_float(const std::string& name, float value) {
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
		}

		void set_mat4(const std::string& name, glm::mat4 mat) {
			int matLoc = glGetUniformLocation(id, name.c_str());
			glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(mat));
		}
	};
}