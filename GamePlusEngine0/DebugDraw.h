#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Texture2D.h"
#include "TextureLoader.h"
#include "FileIO.h"

#include <array>
#include "Renderer.h"
#include "SpritsheetLoader.h"
#include "Defines.h"

#include <glm/gtc/type_ptr.hpp>
namespace IceEngine {


	// Define the shaders code (source code) for drawing a point

	// Setup Geometry and Send to GPU

	// Draw the Point API

	 std::string point_vertex_shader_source = R"(
		#version 330 core
		layout (location = 0) in vec2 aPos;
		uniform float pointSize;
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
		void main()
		{
			gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
			gl_PointSize = pointSize;
			
		}
	)";

	  std::string point_fragment_shader_source = R"(
		#version 330 core
		out vec4 FragColor;
		void main()
		{
			FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		}
	)";


	static void check_compile_errors(GLuint shader, const std::string &type) {
		/*GLint success;
		GLchar infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL Error: " << error << std::endl;
		}*/
		int success;
		char info_log[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, info_log);
			Logger::Instance().Log("Error::Shader::Compilation_Failed" + std::string(info_log), LogLevel::ERROR);
		}
	}

	static void check_linking_errors(GLuint program) {
		GLint success;
		GLchar infoLog[512];
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}

	
	struct DebugPoint
	{
		
		static GLuint shader_program_id;
		static GLuint vertex_buffer_object, vertex_array_object;


		static void Initialize(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
			// Compile shaders and link

			const char *VS = point_vertex_shader_source.c_str();
			const char *FS = point_fragment_shader_source.c_str();

			const GLchar **vs_string = &VS;
			const GLchar **fs_string = &FS;

			GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex_shader, 1, vs_string, NULL);
			glCompileShader(vertex_shader);
			check_compile_errors(vertex_shader, "Vertex Shader");

			GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment_shader, 1, fs_string, NULL);
			check_compile_errors(fragment_shader, "Fragment Shader");
			
			shader_program_id = glCreateProgram();
			glAttachShader(shader_program_id, vertex_shader);
			glAttachShader(shader_program_id, fragment_shader);
			glLinkProgram(shader_program_id);
			check_linking_errors(shader_program_id);

			glDeleteShader(vertex_shader);
			glDeleteShader(fragment_shader);

			// Geometry Setup (VBO, VAO) for a single point
			float vertices[] = { 0.0f, 0.0f };
			
			glGenBuffers(1, &vertex_buffer_object);
			glGenVertexArrays(1, &vertex_array_object);

			glBindVertexArray(vertex_array_object);

			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			glUseProgram(shader_program_id);;
			// Obtain uniform locations
			GLint viewLoc = glGetUniformLocation(shader_program_id, "view");
			GLint projectionLoc = glGetUniformLocation(shader_program_id, "projection");

			// Set uniform values (assuming you have glm::mat4 view and projection)
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		}

		static void Draw(const glm::vec2 &position, float pointSize, const glm::vec3 &color) {
			glUseProgram(shader_program_id);
			glUniform1f(glGetUniformLocation(shader_program_id, "pointSize"), pointSize);
			glUniform3fv(glGetUniformLocation(shader_program_id, "color"), 1, &color[0]);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(position, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shader_program_id, "model"), 1, GL_FALSE, &model[0][0]);
			glBindVertexArray(vertex_array_object);
			glDrawArrays(GL_POINTS, 0, 1);
			glBindVertexArray(0);
			glUseProgram(0);
		}


	};

	GLuint DebugPoint::shader_program_id;
	GLuint DebugPoint::vertex_buffer_object;
	GLuint DebugPoint::vertex_array_object;

	/*	API Usage
	*	glm::mat4 viewMatrix = ...;
		glm::mat4 projectionMatrix = ...;
		DebugPoint::Initialize(viewMatrix, projectionMatrix);
		DebugPoint::Draw(glm::vec2(0.0f, 0.0f, 10.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	* 
	*/


}