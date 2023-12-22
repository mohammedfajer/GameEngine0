#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <GL/glew.h>

#include "Logger.h"

// To Load TTF Files




#define FT_FREETYPE_H
#include "freetype/freetype.h"
#include "ft2build.h"


#include <math.h>
#include <string>
#include <map>

#include "Defines.h"

namespace IceEngine
{
	struct Text
	{
		const char *vertex_shader_source = R"(
			#version 330 core
			layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
			out vec2 TexCoords;

			uniform mat4 projection;

			void main()
			{
				gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
				TexCoords = vertex.zw;
			}
		)";

		const char *fragment_shader_source = R"(
			#version 330 core
			in vec2 TexCoords;
			out vec4 color;

			uniform sampler2D text;
			uniform vec3 textColor;

			void main()
			{    
				vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
				color = vec4(textColor, 1.0) * sampled;
			}  
		)";


		struct Character
		{
			uint32_t textureID;
			glm::ivec2 size;
			glm::ivec2 bearing;
			uint32_t advance;
		};

		std::map<char, Character> characters;

		int fontSize = 48;

		GLuint shader_program;
		GLuint VBO, VAO;

		glm::mat4 projection;
		std::string font_path;

		void setup()
		{
			FT_Library ft;
			if (FT_Init_FreeType(&ft))
			{
				Logger::Instance().Log("Could not init FreeType Library", LogLevel::ERROR);
				return;
			}

			FT_Face face;
			if (FT_New_Face(ft, font_path.c_str(), 0, &face))
			{
				Logger::Instance().Log("Failed to load font", LogLevel::ERROR);
				return;
			}

			FT_Set_Pixel_Sizes(face, 0, fontSize);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

			for (uint8_t c = 0; c < 128; c++)
			{
				// load character glyph
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					Logger::Instance().Log("Failed to load Glyph", LogLevel::ERROR);
					continue;
				}

				// generate texture
				uint32_t texture;

				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer);

				// set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				// now store character for later use
				Character character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					face->glyph->advance.x
				};
				characters.insert(std::pair<char, Character>(c, character));
			}
			

			glBindTexture(GL_TEXTURE_2D, 0);
			FT_Done_Face(face);
			FT_Done_FreeType(ft);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
			glCompileShader(vertex_shader);
			checkShaderCompileError(vertex_shader);

			GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
			glCompileShader(fragment_shader);
			checkShaderCompileError(fragment_shader);

			shader_program = glCreateProgram();
			glAttachShader(shader_program, vertex_shader);
			glAttachShader(shader_program, fragment_shader);
			glLinkProgram(shader_program);
			checkProgramLinkError(shader_program);

			
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}


		void draw(std::string text, float x, float y, float scale, glm::vec3 color)
		{
			glUseProgram(shader_program);
			glUniform3f(glGetUniformLocation(shader_program, "textColor"), color.x, color.y, color.z);
			
			GLint projectionLoc = glGetUniformLocation(shader_program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
			
			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(VAO);

			// iterate through all characters
			std::string::const_iterator c;
			for (c = text.begin(); c != text.end(); c++)
			{
				Character ch = characters[*c];

				float xpos = x + ch.bearing.x * scale;
				float ypos = y - (ch.size.y - ch.bearing.y) * scale;

				float w = ch.size.x * scale;
				float h = ch.size.y * -scale; // flip outside down
				// update VBO for each character
				float vertices[6][4] = {
					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos,     ypos,       0.0f, 1.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },

					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },
					{ xpos + w, ypos + h,   1.0f, 0.0f }
				};
				// render glyph texture over quad
				glBindTexture(GL_TEXTURE_2D, ch.textureID);
				// update content of VBO memory
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// render quad
				glDrawArrays(GL_TRIANGLES, 0, 6);
				// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				x += (ch.advance >> 6) * scale; // bit shift by 6 to get value in pixels (2^6 = 64)
			}
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);

		}
	};


	/* Usage Code
	* 
	*	Text t;
		t.font_path = "./data/fonts/arial.ttf";
		t.fontSize = 48;


		t.projection = m_cameraComponent->projection;
		t.draw("hi", 100, 100, 1.0f, glm::vec2(0.5, 0.8f, 0.2f));

	*
	*/
}