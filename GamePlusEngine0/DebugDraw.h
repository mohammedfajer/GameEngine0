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


namespace IceEngine {

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

	


	// https://faun.pub/draw-circle-in-opengl-c-2da8d9c2c103
	struct DebugPoint
	{
		GLuint shader_program;
		GLuint VBO, VAO;
		float pointSize;
		glm::vec3 color;
		glm::mat4 view;
		glm::mat4 projection;
		bool isRound = false;

		void setup()
		{
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

			GLfloat sizeRange[2] = { 0.0f };
			glGetFloatv(GL_POINT_SIZE_RANGE, sizeRange);
			std::cout << "POINT SIZE = " << sizeRange[0] << " " << sizeRange[1] << std::endl;


			glEnable(GL_PROGRAM_POINT_SIZE);
			// THIS IS THE CODE OF DRAWING CIRCLUR POINTS IN THE SHADER :) 
			glEnable(GL_POINT_SPRITE);


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
			glUniform1f(pointSizeLoc, pointSize);
			
			GLint colorLoc = glGetUniformLocation(shader_program, "color");
			glUniform3f(colorLoc, color.x, color.y, color.z);
		
			glUniform1i(glGetUniformLocation(shader_program, "isRound"), isRound);
		}

	
		
		void draw(int x, int y)
		{
			glUseProgram(shader_program);
			glBindVertexArray(VAO);

			// Set the model matrix uniform
			GLint modelLoc = glGetUniformLocation(shader_program, "model");
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			
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
			FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
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

		void setup(bool outline = true)
		{
			isOutline = outline;

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



	

	struct DebugLine
	{
		GLuint shader_program;
		GLuint VAO;

		float lineThickness = 5.0f;


		void setup(const glm::vec2 &start, const glm::vec2 &end, float thickness)
		{
			lineThickness = thickness;
			glEnable(GL_LINE_SMOOTH);


			// Compile and link shaders
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &line_vertex_shader_source, NULL);
			glCompileShader(vertexShader);
			checkShaderCompileError(vertexShader);

			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &line_fragment_shader_source, NULL);
			glCompileShader(fragmentShader);
			checkShaderCompileError(fragmentShader);

			 shader_program = glCreateProgram();
			glAttachShader(shader_program, vertexShader);
			glAttachShader(shader_program, fragmentShader);
			glLinkProgram(shader_program);
			checkProgramLinkError(shader_program);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);


			glm::vec2 ss = glm::vec2(convert_x_from_world_to_ndc(start.x), convert_y_from_world_to_ndc(start.y));
			glm::vec2 ee = glm::vec2(convert_x_from_world_to_ndc(end.x), convert_y_from_world_to_ndc(end.y));
		
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
		void draw(const glm::vec3 &color, const glm::mat4 &view, const glm::mat4 &projection)
		{
			// Use the shader program
			glUseProgram(shader_program);

			// Set uniform line thickness variable
			GLint lineThicknessLoc = glGetUniformLocation(shader_program, "lineThickness");
			glUniform1f(lineThicknessLoc, lineThickness);

			// Set uniform color variable
			GLint lineColorLoc = glGetUniformLocation(shader_program, "lineColor");
			glUniform3fv(lineColorLoc, 1, glm::value_ptr(color));

			// Set model matrix (identity for simplicity, you can modify it based on your needs)
			//glm::mat4 model = glm::mat4(1.0f);
			// model = glm::translate(glm::mat4(1.0f), glm::vec3(100, 100, 0.0f));
			// model = glm::scale(model, glm::vec3(200.0f)); // Adjust the scaling factor as needed

			 //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

				glm::mat4 model = glm::mat4(1.0f);
				//model = glm::translate(model, glm::vec3(100, 100, 0.0f));
				//model = glm::scale(model, glm::vec3(100)); // Adjust scaling as needed


			// Set up the model, view, and projection matrices
			GLint modelLoc = glGetUniformLocation(shader_program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			GLint viewLoc = glGetUniformLocation(shader_program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			GLint projectionLoc = glGetUniformLocation(shader_program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
			
			
			glLineWidth(lineThickness);;
			glBindVertexArray(VAO);

		


			glDrawArrays(GL_LINES, 0, 2);

		
		
			// Reset line width to default
			glLineWidth(1.0f);
		}
	};
	
	/*
		DebugLine line;
		line.setup({100,100}, {300,300});

		line.draw({1.0,0.0,0.0}, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		
	*/
	

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

	struct DebugRect
	{
		GLuint shader_program;
		GLuint VAO;
		bool isOutline = false;
		float thickness = 1.0f;
		float rotation = 0.0f;


		void draw(int x, int y, int width, int height, const glm::mat4 &view, const glm::mat4 &projection)
		{
			// Translation
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

		
			model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));

			


			// Scale
			model = glm::scale(model, glm::vec3(width, height, 1.0f));


			glUseProgram(shader_program);

			GLint modelLoc = glGetUniformLocation(shader_program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			GLint viewLoc = glGetUniformLocation(shader_program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			GLint projectionLoc = glGetUniformLocation(shader_program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(VAO);
			
			
			if (isOutline)
			{
				// Draw the outline
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glLineWidth(static_cast<float>(thickness));
				glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else
			{
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			
			glBindVertexArray(0);
		}

		void setup()
		{
			// Compile and link shaders
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &rect_vertex_shader_source, NULL);
			glCompileShader(vertexShader);
			checkShaderCompileError(vertexShader);

			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &rect_fragment_shader_source, NULL);
			glCompileShader(fragmentShader);
			checkShaderCompileError(fragmentShader);

			shader_program = glCreateProgram();
			glAttachShader(shader_program, vertexShader);
			glAttachShader(shader_program, fragmentShader);
			glLinkProgram(shader_program);
			checkProgramLinkError(shader_program);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

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
		void draw(int x1, int y1, int x2, int y2, float thickness, const glm::mat4 &view, const glm::mat4 &projection)
		{
			// Calculate angle and distance
			float angleRad = atan2(y2 - y1, x2 - x1);
			float distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

			// Convert angles to degrees
			quad.rotation = angleRad * -180.0 / M_PI;

			// Draw the rectangle
			quad.draw(x1, y1, thickness, distance, view, projection);
		}

	};







	static const char *glsl_vertex_shader = R"(
    #version 330 core
    layout(location = 0) in vec2 v_pos_pattern;
    layout(location = 1) in vec4 v_quad;
    layout(location = 2) in float v_radius;
    layout(location = 3) in float v_thick;

    uniform vec2 u_view_xform;

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

		//vec2 norm_pos   = pos * u_view_xform + vec2(-1.0, +1.0);

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


	struct SDFRoundedRectangle
	{
		GLuint shader_program;
		GLuint VAO, VBO;

		void setup()
		{
			// Compile and link shaders
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &glsl_vertex_shader, NULL);
			glCompileShader(vertexShader);
			checkShaderCompileError(vertexShader);

			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &glsl_fragment_shader, NULL);
			glCompileShader(fragmentShader);
			checkShaderCompileError(fragmentShader);

			shader_program = glCreateProgram();
			glAttachShader(shader_program, vertexShader);
			glAttachShader(shader_program, fragmentShader);
			glLinkProgram(shader_program);
			checkProgramLinkError(shader_program);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

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
				+200.0f, +200.0f, +300.0f, +300.0f, 10.0f, 2.0f,
				+100.0f, +500.0f, 400.0f, +600.0f , 20.0f,  5.0f,
				+600.0f, +100.0f, +700.0f, +300.0f, 0.0f,	4.0f,
				+600.0f, +500.0f, +650.0f, +650.0f, 20.0f, 	2.0f
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
			glVertexAttribPointer(1, 4, GL_FLOAT, false, quad_stride * sizeof(float), (void *)(12 * sizeof(float)));

			glEnableVertexAttribArray(2);
			glVertexAttribDivisor(2, 1);
			glVertexAttribPointer(2, 1, GL_FLOAT, false, quad_stride * sizeof(float), (void *)((12 + 4) * sizeof(float)));

			glEnableVertexAttribArray(3);
			glVertexAttribDivisor(3, 1);
			glVertexAttribPointer(3, 1, GL_FLOAT, false, quad_stride * sizeof(float), (void *)((12 + 5) * sizeof(float)));

			glBindVertexArray(0);

			glUseProgram(shader_program);

			GLint u_view_xform = glGetUniformLocation(shader_program, "u_view_xform");
			glUniform2f(u_view_xform, 2.f / SCREEN_WIDTH, -2.f / SCREEN_HEIGHT);
		}

		void draw(const glm::mat4 &view, const glm::mat4 &projection)
		{
			glUseProgram(shader_program);



			// Translation
			//glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 0.0f));

			glm::mat4 model = glm::mat4(1.0f);

			GLint modelLoc = glGetUniformLocation(shader_program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));



			GLint viewLoc = glGetUniformLocation(shader_program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			GLint projectionLoc = glGetUniformLocation(shader_program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(VAO);

			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 4);

			glBindVertexArray(0);
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
		GLuint shader_program;
		GLuint VAO, VBO;

		float rotation = 0.0f;
		glm::vec3 color;

		float vertices[6];

		void setup() {
			// Compile and link shaders
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &triangle_vertex_shader_source, NULL);
			glCompileShader(vertexShader);
			checkShaderCompileError(vertexShader);

			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &triangle_fragment_shader_source, NULL);
			glCompileShader(fragmentShader);
			checkShaderCompileError(fragmentShader);

			shader_program = glCreateProgram();
			glAttachShader(shader_program, vertexShader);
			glAttachShader(shader_program, fragmentShader);
			glLinkProgram(shader_program);
			checkProgramLinkError(shader_program);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			vertices[0] = -0.5f;
			vertices[1] = -0.5f;
			vertices[2] = 0.5f;
			vertices[3] = -0.5f;
			vertices[4] = 0.0f;
			vertices[5] = 0.5f;

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
			glUseProgram(shader_program);

			glm::mat4 model = glm::mat4(1.0f);

			// Scale the triangle
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 40.0f, 1.0f));

			// Rotate the triangle
			glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));

			// Translate the triangle to its center
			glm::mat4 translationToCenter = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

			// Combine the transformations into the final model matrix
			model = translationToCenter * r * scale;

			GLint modelLoc = glGetUniformLocation(shader_program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			GLint viewLoc = glGetUniformLocation(shader_program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			GLint projectionLoc = glGetUniformLocation(shader_program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(VAO);

			glDrawArrays(GL_TRIANGLES, 0, 3);

			glBindVertexArray(0);
		}
	};

	
	struct DebugArrow {
		
	};

	/*struct ClosedPolygon
	{};*/

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
		GLuint shader_program;
		GLuint VAO, VBO;
		glm::vec3 color;
		std::vector<glm::vec2> vertices;
		bool isOutline;
		float rotation = 0;

		void setup(const std::vector<glm::vec2> &polygonVertices, const glm::vec3 &polygonColor) {
			
			// Compile and link shaders
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &closed_polygon_vertexShaderSource, NULL);
			glCompileShader(vertexShader);
			checkShaderCompileError(vertexShader);

			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &closed_polygon_fragmentShaderSource, NULL);
			glCompileShader(fragmentShader);
			checkShaderCompileError(fragmentShader);

			shader_program = glCreateProgram();
			glAttachShader(shader_program, vertexShader);
			glAttachShader(shader_program, fragmentShader);
			glLinkProgram(shader_program);
			checkProgramLinkError(shader_program);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			
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
			glUseProgram(shader_program);

			
			glm::mat4 model = glm::mat4(1.0f);

			// Scale the triangle
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(50.0, 50.0f, 1.0f));

			// Rotate the triangle
			glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));

			// Translate the triangle to its center
			glm::mat4 translationToCenter = glm::translate(glm::mat4(1.0f), glm::vec3(50,50, 0.0f));

			// Combine the transformations into the final model matrix
			model = translationToCenter * r * scale;

			GLint modelLoc = glGetUniformLocation(shader_program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			GLint viewLoc = glGetUniformLocation(shader_program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			GLint projectionLoc = glGetUniformLocation(shader_program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			GLint colorLoc = glGetUniformLocation(shader_program, "color");
			glUniform3fv(colorLoc, 1, glm::value_ptr(color));

			// Draw the polygon
			glBindVertexArray(VAO);

			isOutline = true;

			if (isOutline)
			{
				glLineWidth(10);
				glDrawArrays(GL_LINE_LOOP , 0, vertices.size());
			}
			else
			{
				glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
			}

			
			glBindVertexArray(0);
			rotation += 0.5f;
		}
	};
	
	struct OpenPolygon
	{};

	
	struct Capsule // special rounded rectangle :)
	{};

	struct DebugEllipse // special circle :)
	{};



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

