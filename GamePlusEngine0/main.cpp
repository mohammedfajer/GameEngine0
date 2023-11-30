

#include <SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#include <stdint.h>
#include <initializer_list>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string vertex_shader = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec2 aTexCoord;

    out vec3 ourColor;
    out vec2 TexCoord;

	void main()
	{
		gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
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
		FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
	}
)";

namespace Engine {

	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Color( ) : r(0), g(0), b(0), a(255) {}

		// Constructor using individual arguments
		Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
			: r(red), g(green), b(blue), a(alpha) {}

		Color(std::initializer_list <uint8_t> values) {
			auto it = values.begin();
			r = (it != values.end()) ? *it : 0; ++it;
			g = (it != values.end()) ? *it : 0; ++it;
			b = (it != values.end()) ? *it : 0; ++it;
			a = (it != values.end()) ? *it : 255;
		}

		std::array<float, 4> flat_list() const {
			std::array<float, 4> result;
			result[0] = r / 255.0f;
			result[1] = g / 255.0f;
			result[2] = b / 255.0f;
			result[3] = a / 255.0f;
			return result;
		}	
	};

	// Wrapper function to use glClearColor with a Color object
	void set_clear_color(Color &c) {
		auto values = c.flat_list();
		glClearColor(values[0], values[1], values[2], values[3]);
	}

	void set_clear_color(std::initializer_list<uint8_t> values) {
		auto c = Color(values);
		set_clear_color(c);
	}

	//// Base case for the variadic template
	//void set_clear_color() {}

	//// Helper function for the variadic template
	//template<typename T, typename... Args>
	//void set_clear_color(const T& arg, Args... args) {
	//	set_clear_color(arg);
	//	set_clear_color(args...);
	//}


    struct Shader {

        uint32_t id = 0;

        Shader(std::string vertex_source, std::string fragment_source)  {
            auto check_error = [](uint32_t shader, std::string type) {
                int success;
                char info_log[512];
                if (type == "program") {
                    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                    if (!success) {
                        glGetShaderInfoLog(shader, 512, NULL, info_log);
                        std::cerr << "Error::Shader::Compilation_Failed\n" << info_log << "\n";
                    }
                }
                else {
                    glGetProgramiv(shader, GL_LINK_STATUS, &success);
                    if (!success) {
                        glGetProgramInfoLog(shader, 512, NULL, info_log);
                        std::cerr << "Error::Shader::Linking_Failed\n" << info_log << "\n";
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

        void use() {glUseProgram(id);}

        void set_bool(std::string &name, bool value) {
            glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
        }

        void set_int (std::string &name, int value) {
            glUniform1i(glGetUniformLocation(id, name.c_str()), value);
        }
        void set_float (std::string &name, float value) {
            glUniform1f(glGetUniformLocation(id, name.c_str()), value);
        }
    };

}


int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
		return (-1);
	}

	SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	if (!window) {
		std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
		return (-1);
	}

	// Create an OpenGL Context
	SDL_GLContext context = SDL_GL_CreateContext(window);

	// Initialize GLEW
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cerr << "GLEW initialization failed: " 
			<< glewGetErrorString(glewError) << std::endl;
		return (-1);
	}

	// Check OpenGL Version
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	// Example glm usage
	glm::vec3 vector1(1.0f, 2.0f, 3.0f);
	glm::vec3 vector2(4.0f, 5.0f, 6.0f);
	glm::vec3 result = vector1 + vector1;

	std::cout << "Result of glm vector addition : (" 
		<< result.x << ", " << result.y << ", " << result.z << ")\n";

	// Set the clear color to green
	//Engine::set_clear_color(0, 128, 0, 255);

    // Geometry
    float vertices [] = {
        // positions            // colors           // texture coords
        +0.5f, +0.5f, +0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,         // top right
        +0.5f, -0.5f, +0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,         // bottom right
        -0.5f, -0.5f, +0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,         //  bottom left
        -0.5f, +0.5f, +0.0f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f          // top left
    };
    uint32_t indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };
    // VAO VBO EBO
    uint32_t VAO, VBO, EBO;
    
    // Generate 
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Copy
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set Vertex Attributes
    // Position
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 8 *sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 8 *sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE, 8 *sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Shader

    // Compile
    uint32_t vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	const char* vs = vertex_shader.c_str();
	const GLchar** vs_string = &vs;
    glShaderSource(vertex_shader_id, 1, vs_string, NULL);
    glCompileShader(vertex_shader_id);

    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, info_log);
        std::cerr << "Error::Shader::Vertex::Compilation_Failed\n" << info_log << "\n";
    }

    uint32_t fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fs = fragment_shader.c_str();
	const GLchar** fs_string = &fs;
    glShaderSource(fragment_shader_id, 1, fs_string, NULL);
    glCompileShader(fragment_shader_id);

    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment_shader_id, 512, NULL, info_log);
        std::cerr << "Error::Shader::Fragment::Compilation_Failed\n" << info_log << "\n";
    }

    // Link
    uint32_t shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader_id);
    glAttachShader(shader_program, fragment_shader_id);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cerr << "Error::ShaderProgram::Linking_Failed\n" << info_log << "\n";
    }
    
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    // Textured Quad
  
    
    // Generate Texture
    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    uint32_t *data = reinterpret_cast<uint32_t*>(stbi_load("./data/container.jpg", &width, &height, &nrChannels, 0));
    if(data)
    {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture \n";
    }
    stbi_image_free(data);




	// Main Loop
	bool quit = false;
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) quit = true;
		}

		// Rendering code goes here
		Engine::set_clear_color({ 0, 128, 0, 255 });
		glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window);
	}
	return (0);
}