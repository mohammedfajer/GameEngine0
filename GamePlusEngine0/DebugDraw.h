#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

#include "SpritsheetLoader.h"
#include "Defines.h"


namespace IceEngine
{

	// Shader
	// Vertex Data
	// Index Data ?
	// Draw
	const char *point_vertex_shader_source = R"(
		#version 330 core
		layout (location = 0) in vec2 aPos;
		
		
		uniform float pointSize;
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main() {
			gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
			gl_PointSize = pointSize;
		}
	)";

	const char *point_fragment_shader_source = R"(
		#version 330 core
		out vec4 FragColor;

		uniform vec3 color;

		uniform bool isRound;

		void main() {

			if(isRound)
			{
				// Calculate distance from the center of the point
				float distance = length(gl_PointCoord - vec2(0.5));

				// If the distance is greater than 0.5, discard the fragment
				if (distance > 0.5) {
					discard;
				}
			}
			// Otherwise, set the fragment color
			FragColor = vec4(color, 1.0);
		}
	)";

	// Function to check shader compilation errors
	void checkShaderCompileError(GLuint shader) {
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
	void checkProgramLinkError(GLuint program) {
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



	struct DebugPoint
	{
		GLuint shader_program;
		GLuint VBO, VAO;
		float pointSize;
		glm::vec3 color;
		glm::mat4 view;
		glm::mat4 projection;
		bool isRound = false;

		void setup_point(const glm::mat4 &view, const glm::mat4 &projection)
		{


			this->view = view;
			this->projection = projection;

			GLfloat sizeRange[2] = { 0.0f };
			glGetFloatv(GL_POINT_SIZE_RANGE, sizeRange);

			std::cout << "POINT SIZE = " << sizeRange[0] << " " << sizeRange[1] << std::endl;


			glEnable(GL_PROGRAM_POINT_SIZE);



			// THIS IS THE CODE OF DRAWING CIRCLUR POINTS IN THE SHADER :) 
			glEnable(GL_POINT_SPRITE);

			GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex_shader, 1, &point_vertex_shader_source, NULL);
			glCompileShader(vertex_shader);
			checkShaderCompileError(vertex_shader);

			GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment_shader, 1, &point_fragment_shader_source, NULL);
			glCompileShader(fragment_shader);
			checkShaderCompileError(fragment_shader);

			shader_program = glCreateProgram();
			glAttachShader(shader_program, vertex_shader);
			glAttachShader(shader_program, fragment_shader);
			glLinkProgram(shader_program);
			checkProgramLinkError(shader_program);

			glUseProgram(shader_program);

			float vertices[] = { 0.0f, 0.0f };

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(0);

			// Set the resolution uniform


			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				// Handle or print the error
				std::cerr << "OpenGL Error: " << error << std::endl;
			}
			GLint pointSizeLoc = glGetUniformLocation(shader_program, "pointSize");
			if (pointSizeLoc != -1) {
				glUniform1f(pointSizeLoc, pointSize);
			}
			else {
				std::cerr << "projView matrix uniform not found in the shader!" << std::endl;
			}
			GLint colorLoc = glGetUniformLocation(shader_program, "color");
			if (colorLoc != -1) {
				glUniform3f(colorLoc, color.x, color.y, color.z);
			}
			else {
				std::cerr << "projView matrix uniform not found in the shader!" << std::endl;
			}

			glUniform1i(glGetUniformLocation(shader_program, "isRound"), isRound);
		}

		void draw(int x, int y, const glm::mat4 &view)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

			glUseProgram(shader_program);
			glBindVertexArray(VAO);


			// Set the model matrix uniform
			GLint modelLoc = glGetUniformLocation(shader_program, "model");
			if (modelLoc != -1) {
				// Assuming you're using a simple 2D translation matrix
				glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			}
			else {
				std::cerr << "Model matrix uniform not found in the shader!" << std::endl;
			}
			// Set the projView matrix uniform
			GLint viewLoc = glGetUniformLocation(shader_program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			GLint projectionLoc = glGetUniformLocation(shader_program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glDrawArrays(GL_POINTS, 0, 1);
			glBindVertexArray(0);
			glUseProgram(0);
		}
	};


	const char *circle_vertex_shader_source = R"(
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	gl_PointSize = 10.0f;
}
)";

	const char *circle_fragment_shader_source = R"(
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(176/255.0f, 55/255.0f, 97/255.0f, 1.0f);
}
)";
	
	struct DebugCircle
	{
		GLuint shader_program;
		GLuint VAO;

		float radius;
		int vCount;
		std::vector<glm::vec3> vertices;
		bool isOutline = true;

		void setup()
		{
			GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex_shader, 1, &circle_vertex_shader_source, NULL);
			glCompileShader(vertex_shader);
			checkShaderCompileError(vertex_shader);

			GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment_shader, 1, &circle_fragment_shader_source, NULL);
			glCompileShader(fragment_shader);
			checkShaderCompileError(fragment_shader);

			shader_program = glCreateProgram();
			glAttachShader(shader_program, vertex_shader);
			glAttachShader(shader_program, fragment_shader);
			glLinkProgram(shader_program);
			checkProgramLinkError(shader_program);


			build(vCount);

			glUseProgram(shader_program);

			// buffer
			GLuint VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

		
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW); 

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

			glEnableVertexAttribArray(0);


			//// Enable point size and point sprite
			//glEnable(GL_PROGRAM_POINT_SIZE);
			//glEnable(GL_POINT_SPRITE);

			//// Check the point size range
			//GLfloat sizeRange[2] = { 0.0f };
			//glGetFloatv(GL_POINT_SIZE_RANGE, sizeRange);
			//std::cout << "POINT SIZE RANGE = " << sizeRange[0] << " " << sizeRange[1] << std::endl;

		}

		void build(int vCount)
		{
			float angle = 360.0f / vCount;

			int triangleCount = vCount - 2;

			std::vector<glm::vec3> temp;

			for (int i = 0; i < vCount; i++)
			{
				float currentAngle = angle * i;
				float x = radius * cos(glm::radians(currentAngle));
				float y = radius * sin(glm::radians(currentAngle));
				float z = 0.0f;
				temp.push_back(glm::vec3(x, y, z));
			}

			if (isOutline)
			{
				vertices = temp;
				return;
			}

			for (int i = 0; i < triangleCount; i++)
			{
				vertices.push_back(temp[0]);
				vertices.push_back(temp[i + 1]);
				vertices.push_back(temp[i + 2]);
			}
		}

		


		void draw(int x, int y, const glm::mat4 &view, const glm::mat4 &projection)
		{
			
			glUseProgram(shader_program);

			// Set model matrix with translation and scaling
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
			model = glm::scale(model, glm::vec3(100.0f)); // Adjust the scaling factor as needed


			// Pass matrices to the shader
			GLint modelLoc = glGetUniformLocation(shader_program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			GLint viewLoc = glGetUniformLocation(shader_program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			GLint projectionLoc = glGetUniformLocation(shader_program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(VAO);
			
			if (isOutline)
			{
				glDrawArrays(GL_LINE_LOOP, 0, vertices.size());
			}
			else
			{
				glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			}
			
		}

	};

	

	

}

