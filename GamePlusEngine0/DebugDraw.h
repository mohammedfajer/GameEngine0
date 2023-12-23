#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "SpritsheetLoader.h"
#include "Defines.h"
#include <cmath>
#include <SDL_stdinc.h>
#include "InputManager.h"
#include "Logger.h"
#include "Shader.h"

namespace IceEngine {



	// TODO(mo): Reduce the shader code
	// Ensure Each Class is Customized and Tested
	

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
			if(isRound) {
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

	// https://faun.pub/draw-circle-in-opengl-c-2da8d9c2c103
	struct DebugPoint {
		Shader shader;
		GLuint VBO, VAO;
		float pointSize;
		glm::vec3 color;
		glm::mat4 view;
		glm::mat4 projection;
		bool isRound = false;

		void setup() {
			shader.LoadShaderFromString(point_vertex_shader_source, point_fragment_shader_source);
			GLfloat sizeRange[2] = { 0.0f };
			glGetFloatv(GL_POINT_SIZE_RANGE, sizeRange);
			std::cout << "POINT SIZE = " << sizeRange[0] << " " << sizeRange[1] << std::endl;

			glEnable(GL_PROGRAM_POINT_SIZE);

			// THIS IS THE CODE OF DRAWING CIRCLUR POINTS IN THE SHADER :) 
			glEnable(GL_POINT_SPRITE);

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

			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				// Handle or print the error
				std::cerr << "OpenGL Error: " << error << std::endl;
			}
			shader.Bind();
			shader.SetFloat("pointSize", pointSize);
			shader.SetVec3("color", color);
			shader.SetInt("isRound", isRound);
		}

		void draw(int x, int y) {
			shader.Bind();
			glBindVertexArray(VAO);
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
			shader.SetMat4("model", model);
			shader.SetMat4("view", view);
			shader.SetMat4("projection", projection);
			glDrawArrays(GL_POINTS, 0, 1);
			glBindVertexArray(0);
			shader.UnBind();
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
			FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}
	)";
	
	struct DebugCircle {
		Shader shader;
		GLuint VAO;
		float radius;
		int vCount;
		std::vector<glm::vec3> vertices;
		bool isOutline = true;

		void setup(bool outline = true) {
			isOutline = outline;

			shader.LoadShaderFromString(circle_vertex_shader_source, circle_fragment_shader_source);
			build(vCount);

			GLuint VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW); 
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);
		}

		void build(int vCount) {
			float angle = 360.0f / vCount;
			int triangleCount = vCount - 2;
			std::vector<glm::vec3> temp;

			for (int i = 0; i < vCount; i++) {
				float currentAngle = angle * i;
				float x = radius * cos(glm::radians(currentAngle));
				float y = radius * sin(glm::radians(currentAngle));
				float z = 0.0f;
				temp.push_back(glm::vec3(x, y, z));
			}

			if (isOutline) {
				vertices = temp;
				return;
			}

			for (int i = 0; i < triangleCount; i++) {
				vertices.push_back(temp[0]);
				vertices.push_back(temp[i + 1]);
				vertices.push_back(temp[i + 2]);
			}
		}

		void draw(int x, int y, const glm::mat4 &view, const glm::mat4 &projection) {
			shader.Bind();
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
			model = glm::scale(model, glm::vec3(100.0f));
	
			shader.SetMat4("model", model);
			shader.SetMat4("view", view);
			shader.SetMat4("projection", projection);

			glBindVertexArray(VAO);
			
			if (isOutline) {
				glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(vertices.size()));
			} else {
				glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
			}

			shader.UnBind();
		}
	};


	const char *line_vertex_shader_source = R"(
		#version 330 core

		layout (location = 0) in vec2 aPos;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		out vec2 fragCoord;

		void main()
		{
			gl_Position = projection * view * model * vec4(aPos.x, aPos.y, 0.0, 1.0);
		

		
			fragCoord = aPos;
		}
	)";

	const char *line_fragment_shader_source = R"(
		#version 330 core

		out vec4 FragColor;
		uniform vec3 lineColor;
		uniform float lineThickness;
		
		in vec2 fragCoord;

		void main()
		{
			float thickness = lineThickness;
			
			float distance = length(fragCoord);
			float alpha = smoothstep(0.5 * thickness, 0.5 * thickness + 0.01, distance);
			

			FragColor = vec4(1.0, 0.0, 0.0, alpha);
		}
	)";

	struct DebugLine {
		Shader shader;
		GLuint VAO;
		float lineThickness = 5.0f;

		void setup(const glm::vec2 &start, const glm::vec2 &end, float thickness) {
			lineThickness = thickness;
			glEnable(GL_LINE_SMOOTH);

			shader.LoadShaderFromString(line_vertex_shader_source, line_fragment_shader_source);

			glm::vec2 ss = glm::vec2(convert_x_from_world_to_ndc(start.x),
			                         convert_y_from_world_to_ndc(start.y));
			glm::vec2 ee = glm::vec2(convert_x_from_world_to_ndc(end.x),
			                         convert_y_from_world_to_ndc(end.y));
		
			float vertices[] = { start.x, start.y, end.x, end.y };

			// Create Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
			GLuint VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			// Bind the VAO
			glBindVertexArray(VAO);

			// Bind and set vertex buffer data
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Set vertex attribute pointers
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);

			// Unbind VAO and VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void draw(const glm::vec3 &color, const glm::mat4 &view, const glm::mat4 &projection) {
			shader.Bind();
			shader.SetFloat("lineThickness", lineThickness);
			shader.SetVec3("lineColor", color);
			glm::mat4 model = glm::mat4(1.0f);
			shader.SetMat4("model", model);
			shader.SetMat4("view", view);
			shader.SetMat4("projection", projection);
			glLineWidth(lineThickness);;
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINES, 0, 2);
			// Reset line width to default
			glLineWidth(1.0f);
			shader.UnBind();
		}
	};
	

	const char *rect_vertex_shader_source = R"(
		#version 330 core
		layout (location = 0) in vec2 aPos;
	
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
	
		void main()
		{
			gl_Position = projection * view * model * vec4(aPos.x, aPos.y, 0.0, 1.0);
		}
	)";

	const char *rect_fragment_shader_source = R"(
		#version 330 core
		out vec4 FragColor;
		void main()
		{
			FragColor = vec4(252/255.0f, 186/255.0f, 3/255.0f, 1.0);
		}
	)";

	struct DebugRect {
		GLuint VAO;
		bool isOutline = false;
		float thickness = 1.0f;
		float rotation = 0.0f;
		Shader shader;
	
		void draw(int x, int y, int width, int height,
		          const glm::mat4 &view, const glm::mat4 &projection) {
			shader.Bind();

			// Translation
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
			model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
			// Scale
			model = glm::scale(model, glm::vec3(width, height, 1.0f));

			shader.SetMat4("model", model);
			shader.SetMat4("view", view);
			shader.SetMat4("projection", projection);
	
			glBindVertexArray(VAO);
			if (isOutline) {
				// Draw the outline
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glLineWidth(static_cast<float>(thickness));
				glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glLineWidth(1.0f);
			} else {
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			
			glBindVertexArray(0);

			shader.UnBind();
		}

		void setup() {
			shader.LoadShaderFromString(rect_vertex_shader_source, rect_fragment_shader_source);
			float vertices[] = {
				0.5f, 0.5f,   // top right
				0.5f, -0.5f,  // bottom right
				-0.5f, -0.5f, // bottom left
				-0.5f, 0.5f   // top left
			};

			unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

			// Create Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
			GLuint VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			// Bind the VAO
			glBindVertexArray(VAO);

			// Bind and set vertex buffer data
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Set vertex attribute pointers
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);

			// Create Element Buffer Object (EBO) and bind it
			GLuint EBO;
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			// Unbind VAO
			glBindVertexArray(0);
		}
	};


	struct DebugLineQuad {
		DebugRect quad;
		void setup() { quad.setup(); }
		void draw(int x1, int y1, int x2, int y2,
		          float thickness, const glm::mat4 &view, const glm::mat4 &projection) {
			// Calculate angle and distance

			float dx = static_cast<float>(x2 - x1);
			float dy = static_cast<float>(y2 - y1);

			float angleRad = static_cast<float> (atan2f(dy,dx));
			float distance = static_cast<float> (sqrtf(static_cast<float>(pow(dx, 2)) + static_cast<float>(pow(dy, 2))));

			// Convert angles to degrees
			quad.rotation = static_cast<float>( angleRad * -180.0f / M_PI );

			// Calculate the left corner position of the quad
			float quadX = x1 + (dx) * 0.5f;
			float quadY = y1 + (dy) * 0.5f;

			// Draw the rectangle with left corner at (x1, y1)
			quad.draw((int)quadX, (int)quadY, (int)thickness, (int)distance, view, projection);
		}
	};

	static const char *glsl_vertex_shader = R"(
		#version 330 core
		layout(location = 0) in vec2 v_pos_pattern;
		layout(location = 1) in vec4 v_quad;
		layout(location = 2) in float v_radius;
		layout(location = 3) in float v_thick;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
	
		out vec2 f_center;
		out vec2 f_half_dim;
		out vec2 f_pos;
		out float f_radius;
		out float f_thick;
	
		void main()
		{
			vec2 center     = (v_quad.xy + v_quad.zw) * 0.5;
			vec2 half_dim   = (v_quad.zw - v_quad.xy) * 0.5;
			vec2 pos        = center + half_dim * v_pos_pattern;
		
	
			// Apply transformations
			vec4 worldPos   = view * model * vec4(pos, 0.0, 1.0);
			vec2 norm_pos   = (projection * worldPos).xy;
			gl_Position     = vec4(norm_pos, 0.0, 1.0);
	
			f_pos           = pos;
			f_center        = center;
			f_half_dim      = half_dim;
			f_radius        = v_radius;
			f_thick         = v_thick;
		}
	)";

	static const char *glsl_fragment_shader = R"(
		#version 330
	
		in vec2 f_center;
		in vec2 f_half_dim;
		in vec2 f_pos;
		in float f_radius;
		in float f_thick;
	
		out vec4 out_color;
	
		void main()
		{
			// setup params
			float r = f_radius;
			float soft = 1.0f;
			float soft_padding = max(0, soft * 2 - 1);
			float thickness = f_thick;
	
			// Calculate rectangle's euclidean signed distance
			vec2 d2          = abs(f_pos - f_center) -  f_half_dim + vec2(r, r) + vec2(soft, soft);
			float d_neg      = min(max(d2.x, d2.y), 0);
			float d_pos      = length(max(d2, vec2(0, 0)));
			float d_rect     = d_neg + d_pos;
	
			// Calculate apply extra radius
			float d_rounded = d_rect - r;
	
			// final distance
			float d = d_rounded;
	
			// distance response curve
			float half_thickness = thickness * 0.5;
			float d_mir = abs(d + half_thickness) - half_thickness;
	
			// Mix with fill color inside the rounded rectangle
			vec3 fillColor = vec3(0.0, 1.0, 0.0);  // Adjust the fill color as needed
			vec3 mixedColor = mix(fillColor, vec3(1.0, 1.0, 1.0), smoothstep(soft, -soft, d_mir));
	
			// Check if the fragment is outside the edge radius
			if (d > 0.0) {
				out_color = vec4(0.0, 0.0, 0.0, 0.0);  // Transparent outside the edge radius
			} else {
				out_color = vec4(mixedColor, 1.0);
			}
		}
	)";

	static const char *sdfrounded_glsl_vertex_shader = R"(
		#version 330 core
		layout(location = 0) in vec2 v_pos_pattern;
	
		uniform vec2 u_position;
		uniform vec2 u_size;
		uniform float u_radius;
		uniform float u_thickness;
	
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
	
		out vec2 f_center;
		out vec2 f_half_dim;
		out vec2 f_pos;
		out float f_radius;
		out float f_thick;
	
		void main()
		{
			vec2 center     = (u_position + (u_position + u_size)) * 0.5;
			vec2 half_dim   = ((u_position + u_size) - u_position) * 0.5;
			vec2 pos        = center + half_dim * v_pos_pattern;
		
			// Apply transformations
			vec4 worldPos   = view * model * vec4(pos, 0.0, 1.0);
			vec2 norm_pos   = (projection * worldPos).xy;
			gl_Position     = vec4(norm_pos, 0.0, 1.0);
	
			f_pos           = pos;
			f_center        = center;
			f_half_dim      = half_dim;
			f_radius        = u_radius;
			f_thick         = u_thickness;
		}
	)";

	class RoundedRect {
		public:
			void Setup() {
				setupShader();
				setupGeometry();
		
				GLenum error = glGetError();
				if (error != GL_NO_ERROR) {
					std::cerr << "OpenGL error: " << error << std::endl;
				}
			}
			RoundedRect() = default;
		
			~RoundedRect() {
				glDeleteVertexArrays(1, &VAO);
				glDeleteBuffers(1, &VBO);
			}
		
			void setSize(const glm::vec2& newSize) { size = newSize; }
		
			void setRadius(float newRadius) { radius = newRadius; }
		
			void setThickness(float newThickness) { thickness = newThickness; }
		
			void draw(const glm::vec2& pos, const glm::mat4& view, const glm::mat4& projection) {
				shader.Bind();
		
				shader.SetVec2("u_position", pos);
				shader.SetVec2("u_size", size);
				shader.SetFloat("u_radius", radius);
				shader.SetFloat("u_thickness", thickness);
		
				// Set transformation matrices
				glm::mat4 model = glm::mat4(1.0f);
	
				shader.SetMat4("model", model);
				shader.SetMat4("view", view);
				shader.SetMat4("projection", projection);
		
				glBindVertexArray(VAO);
				glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
				glBindVertexArray(0);
		
				shader.UnBind();
			}
	
		private:
			
			GLuint VAO, VBO;
		
			glm::vec2 size;
			float radius = 1.0f;
			float thickness = 1.0f;
			Shader shader;
		
			void setupShader() {
				shader.LoadShaderFromString(sdfrounded_glsl_vertex_shader, glsl_fragment_shader);
			}
		
			void setupGeometry() {
				uint64_t quad_stride = 6;
		
				float vertices[] = {
					// [0]: pattern of quad
					-1.0f, +1.0f,
					+1.0f, +1.0f,
					-1.0f, -1.0f,
					+1.0f, +1.0f,
					-1.0f, -1.0f,
					+1.0f, -1.0f,
				};
		
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
		
				glBindVertexArray(VAO);
		
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
		
				// Position attribute
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), 0);
				glBindVertexArray(0);
			}
	};


	struct SDFRoundedRectangle {
		GLuint VAO, VBO;
		glm::vec2 size;
		float radius = 1.0f;
		float thickness = 1.0f;
		Shader shader;

		void setup() {
			shader.LoadShaderFromString(glsl_vertex_shader, glsl_fragment_shader);

			uint64_t quad_stride = 6;

			// Set up the rectangle's geometry
			float vertices[] = {
				// [0]: pattern of quad
				-1.0f, +1.0f,
				+1.0f, +1.0f,
				-1.0f, -1.0f,
				+1.0f, +1.0f,
				-1.0f, -1.0f,
				+1.0f, -1.0f,

				// [12]: four quad specifiers

				0.0f, 0.0f, size.x, size.y, radius, thickness
				//-1.0f, 1.0f, 1.0f, 1.0f, radius, thickness
			};

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

			// Position attribute
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), 0);

			glEnableVertexAttribArray(1);
			glVertexAttribDivisor(1, 1);
			glVertexAttribPointer(1, 4, GL_FLOAT, false, static_cast<GLsizei>(quad_stride * sizeof(float)), (void *)(12 * sizeof(float)));

			glEnableVertexAttribArray(2);
			glVertexAttribDivisor(2, 1);
			glVertexAttribPointer(2, 1, GL_FLOAT, false, static_cast<GLsizei>(quad_stride * sizeof(float)), (void *)((12 + 4) * sizeof(float)));

			glEnableVertexAttribArray(3);
			glVertexAttribDivisor(3, 1);
			glVertexAttribPointer(3, 1, GL_FLOAT, false, static_cast<GLsizei>(quad_stride * sizeof(float)), (void *)((12 + 5) * sizeof(float)));

			glBindVertexArray(0);

			shader.Bind();

			shader.SetVec2("u_view_xform", glm::vec2(2.0f / SCREEN_WIDTH, -2.0f / SCREEN_HEIGHT));
		}

		void draw(const glm::vec2& pos, const glm::mat4 &view, const glm::mat4 &projection) {
			shader.Bind();
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			shader.SetMat4("model", model);
			shader.SetMat4("view", view);
			shader.SetMat4("projection", projection);
			glBindVertexArray(VAO);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
			glBindVertexArray(0);
			shader.UnBind();
		}
	};


	const char *triangle_vertex_shader_source = R"(
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

	const char *triangle_fragment_shader_source = R"(
		#version 330 core
		out vec4 FragColor;
		void main() {
			FragColor = vec4(0.3, 0.5, 0.2, 1.0);
		}
	)";

	struct DebugTriangle {
		
		GLuint VAO, VBO;
		float rotation = 0.0f;
		glm::vec3 color;
		glm::vec2 size  = {10.0f, 10.0f};
		Shader shader;

		void setup() {
			shader.LoadShaderFromString(triangle_vertex_shader_source, triangle_fragment_shader_source);

			float vertices[] = {
				-0.5f  , -0.5f,
				+ 0.5f , -0.5f,
				+ 0.0f , +0.05f
			};

			// Create Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			// Bind the VAO
			glBindVertexArray(VAO);

			// Bind and set vertex buffer data
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Set vertex attribute pointers
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);

			// Unbind VAO
			glBindVertexArray(0);
		}

		void draw(int x, int y, const glm::mat4 &view, const glm::mat4 &projection) {
			shader.Bind();

			glm::mat4 model = glm::mat4(1.0f);

			// Scale the triangle
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

			// Rotate the triangle
			glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));

			// Translate the triangle to its center
			glm::mat4 translationToCenter = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

			// Combine the transformations into the final model matrix
			model = translationToCenter * r * scale;

			shader.SetMat4("model", model);
			shader.SetMat4("view", view);
			shader.SetMat4("projection", projection);

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);

			shader.UnBind();
		}
	};

	struct DebugArrow {
		DebugTriangle triangle;
		DebugLineQuad quad;
		void setup() {
			triangle.setup();
			quad.setup();
		}
		void draw(int x, int y, int dx, int dy, const glm::mat4 &view, const glm::mat4 &projection) {
			quad.draw(x,y, x+dx, y+dy, 3.0f, view, projection);
			if(InputManager::Instance().IsKeyPressed(SDL_SCANCODE_U)) {
				Logger::Instance().Log(LogLevel::SUCCESS, "Rotation %", triangle.rotation);
			}
			triangle.rotation = 315.0f;
			triangle.size = {20, 20};
			triangle.draw(x+dx, y+dy, view, projection);
		}
	};

	// Shader source code (replace with your actual shader code)
	const char *closed_polygon_vertexShaderSource = R"(
		#version 330 core
		layout (location = 0) in vec2 aPos;
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
		void main() {
			gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
		}
	)";

	const char *closed_polygon_fragmentShaderSource = R"(
		#version 330 core
		out vec4 FragColor;
		uniform vec3 color;
		void main() {
			FragColor = vec4(color, 1.0);
		}
	)";


	struct ClosedPolygon {
		
		GLuint VAO, VBO;
		glm::vec3 color;
		std::vector<glm::vec2> vertices;
		bool isOutline;
		float rotation = 0;

		Shader shader;

		void setup(const std::vector<glm::vec2> &polygonVertices, const glm::vec3 &polygonColor){
			shader.LoadShaderFromString(closed_polygon_vertexShaderSource,  closed_polygon_fragmentShaderSource);

			vertices = polygonVertices;
			color = polygonColor;

			// Create Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			// Bind the VAO
			glBindVertexArray(VAO);

			// Bind and set vertex buffer data
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);

			// Set vertex attribute pointers
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);

			// Unbind VAO
			glBindVertexArray(0);
		}

		void draw(const glm::mat4 &view, const glm::mat4 &projection) {
			shader.Bind();
			
			glm::mat4 model = glm::mat4(1.0f);

			// Scale the triangle
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(50.0, 50.0f, 1.0f));

			// Rotate the triangle
			glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));

			// Translate the triangle to its center
			glm::mat4 translationToCenter = glm::translate(glm::mat4(1.0f), glm::vec3(50,50, 0.0f));

			// Combine the transformations into the final model matrix
			model = translationToCenter * r * scale;

			shader.SetMat4("model", model);
			shader.SetMat4("view", view);
			shader.SetMat4("projection", projection);
			shader.SetVec3("color", color);

			// Draw the polygon
			glBindVertexArray(VAO);

			isOutline = true;
			if (isOutline) {
				glLineWidth(10);
				glDrawArrays(GL_LINE_LOOP , 0, static_cast<GLsizei>(vertices.size()));
				glLineWidth(1.0f);
			} else {
				glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vertices.size()));
			}

			glBindVertexArray(0);
			rotation += 0.5f;
			shader.UnBind();
		}
	};
	
	// Effects
	// Borders , Outlines
	// Gradients
	// Fading
	// Roundness
	// Textured, Animated
	// Curves
	// Fonts,  Font styles
	// Textures, Animated Textures
	// Console
	// Game UI 
}