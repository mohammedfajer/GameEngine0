

#include <SDL.h>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <stdint.h>
#include <initializer_list>
#include <array>

#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cmath>
#include <vector>

#include <random>

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

        void set_bool(const std::string &name, bool value) {
            glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
        }

        void set_int (const std::string &name, int value) {
            glUniform1i(glGetUniformLocation(id, name.c_str()), value);
        }
        void set_float (const std::string &name, float value) {
            glUniform1f(glGetUniformLocation(id, name.c_str()), value);
        }

        void set_mat4 (const std::string &name, glm::mat4 mat) {
			int matLoc = glGetUniformLocation(id, name.c_str());
			glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(mat));
        }
    };


    uint32_t load_texture(const std::string& filePath, GLenum format = GL_RGB, GLenum wrapS = GL_REPEAT,
         GLenum wrapT = GL_REPEAT, GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR) {
		uint32_t texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

		if (data) {
			GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
			GLenum internalFormat = (format == GL_RGBA) ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cerr << "Failed to load texture from " << filePath << "\n";
		}

		stbi_image_free(data);
		return texture;
    }

	struct OrthographicCamera {
		glm::mat4 projection;
		glm::vec2 position;
		float zoom;
		
		OrthographicCamera(float left, float right, float bottom, float top, float zoomLevel = 1.0f)
			: position(0.0f, 0.0f), zoom(zoomLevel) {
			set_projection(left, right, bottom, top);
		}

		void set_projection(float left, float right, float bottom, float top) {
			projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		}

		glm::mat4 get_view_matrix() const {
			auto R = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, -position.y, 1.0)) * 
			glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, 1.0f));
			return (R);
		}
	};

}


struct Sprite {
	glm::vec2 position;
	glm::vec2 scale;
	
	glm::mat4 model;
	float rotation;
	
	uint32_t texture;
	std::string name;

	Sprite(const glm::vec2& position, const glm::vec2& scale, uint32_t texture, std::string name = "untitled")
	: position(position), scale(scale), texture(texture), rotation(0.0f), name(name) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(position, 0.0f));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scale, 1.0f));
	}
};

// Function to check if two sprites are too close to each other
bool areSpritesTooClose(const Sprite& sprite1, const Sprite& sprite2, float minDistance) {
	float distance = glm::length(sprite1.position - sprite2.position);
	return distance < minDistance;
}

// Function to generate random non-overlapping positions
glm::vec2 generateRandomPosition(const std::vector<Sprite>& sprites, float minX, float maxX,
	float minY, float maxY, float minDistance) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> disX(minX, maxX);
	std::uniform_real_distribution<float> disY(minY, maxY);

	glm::vec2 randomPosition;
	bool tooClose;

	do {
		tooClose = false;
		randomPosition.x = disX(gen);
		randomPosition.y = disY(gen);

		for (const auto& sprite : sprites) {
			if (areSpritesTooClose(sprite, { randomPosition, glm::vec2(0.0f, 0.0f), 0 }, minDistance)) {
				tooClose = true;
				break;
			}
		}
	} while (tooClose);

	return randomPosition;
}



#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_LIGHT_BLUE  "\x1b[94m"
#define ANSI_COLOR_RED     "\x1b[31m"

enum class LogLevel {
	SUCCESS,
	INFO,
	ERROR
};

void log(LogLevel level, const std::string& message) {
	switch (level) {
		case LogLevel::SUCCESS:
			std::cout << ANSI_COLOR_GREEN << "[SUCCESS] " << message << ANSI_COLOR_RESET << std::endl;
			break;
		case LogLevel::INFO:
			std::cout << ANSI_COLOR_LIGHT_BLUE << "[INFO] " << message << ANSI_COLOR_RESET << std::endl;
			break;
		case LogLevel::ERROR:
			std::cerr << ANSI_COLOR_RED << "[ERROR] " << message << ANSI_COLOR_RESET << std::endl;
			break;
	}
}

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768




int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
		return (-1);
	}

	SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
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

	log(LogLevel::INFO, "Info Text");
	log(LogLevel::SUCCESS, "Success Text");
	log(LogLevel::ERROR, "Error Text");

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
    Engine::Shader shader(vertex_shader, fragment_shader);

	// Main Loop
	bool quit = false;

    // Camera 
	Engine::OrthographicCamera camera (0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	camera.position = glm::vec2(0.0f, 0.0f);
	camera.zoom = 1.0f;

    // Object position and scale
    float objectX = 100.0f;
    float objectY = 100.0f;
    float objectScaleX = 48.0;
    float objectScaleY = 46.0;
    float rotationAngle = 0.0f;

	std::vector<Sprite> sprites;
	uint32_t coinTexture = Engine::load_texture("./data/coin.png");
	uint32_t backgroundTexture = Engine::load_texture("./data/bg.png");
	
	//Sprite background(glm::vec2(800/2, 600/2), glm::vec2(800, 600), backgroundTexture, "bg");
	//sprites.push_back(background);

	/*sprites.push_back(Sprite(glm::vec2(100.0f, 100.0f), glm::vec2(50.0f, 50.0f), coinTexture));
	sprites.push_back(Sprite(glm::vec2(150.0f, 100.0f), glm::vec2(50.0f, 50.0f), coinTexture));
	sprites.push_back(Sprite(glm::vec2(200.0f, 100.0f), glm::vec2(50.0f, 50.0f), coinTexture));
	sprites.push_back(Sprite(glm::vec2(250.0f, 100.0f), glm::vec2(50.0f, 50.0f), coinTexture));

	sprites.push_back(Sprite(glm::vec2(100.0f, 250.0f), glm::vec2(50.0f, 50.0f), coinTexture));
	sprites.push_back(Sprite(glm::vec2(150.0f, 250.0f), glm::vec2(50.0f, 50.0f), coinTexture));
	sprites.push_back(Sprite(glm::vec2(200.0f, 250.0f), glm::vec2(50.0f, 50.0f), coinTexture));
	sprites.push_back(Sprite(glm::vec2(250.0f, 250.0f), glm::vec2(50.0f, 50.0f), coinTexture));*/


	for (int i = 0; i < 25; ++i) {
		glm::vec2 randomPosition = generateRandomPosition(sprites, 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, 50.0f);
		sprites.push_back(Sprite(randomPosition, glm::vec2(50.0f, 50.0f), coinTexture));
	}


	sprites.push_back(Sprite(glm::vec2(300, 350), glm::vec2(50, 50), Engine::load_texture("./data/plane_1_pink.png"), "player"));




	// Enable blending
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) quit = true;
		}

		// Get the current state of the keyboard
		const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

		glm::mat4 projection = camera.projection;
		glm::mat4 view = camera.get_view_matrix();

		for (Sprite &sprite: sprites) {
			if (sprite.name != "bg" && sprite.name != "player")
			{
				auto time = SDL_GetTicks() * 0.01f;
				sprite.position.y += 0.15f * sinf(time); // Adjust the multiplier as needed
				sprite.position.x += 0.15f * sinf(time); // Adjust the multiplier as needed
				sprite.model = glm::mat4(1.0f);
				sprite.model = glm::translate(sprite.model, glm::vec3(sprite.position, 0.0f));
				sprite.model = glm::scale(sprite.model, glm::vec3(sprite.scale, 1.0f));
			}

			if (sprite.name == "player") {

				const float speed = 50; // Adjust the speed as needed
				auto time = SDL_GetTicks() * 0.01f;
				static const Uint8* prevKeyboardState = nullptr;
				bool move = false;
				// Check for key releases
				if (prevKeyboardState) {
					if (!prevKeyboardState[SDL_SCANCODE_W] && keyboardState[SDL_SCANCODE_W]) {
						// Key W released
						std::cout << "Key W released\n";
						 move = false;
					}
					if (!prevKeyboardState[SDL_SCANCODE_S] && keyboardState[SDL_SCANCODE_S]) {
						// Key S released
						std::cout << "Key S released\n";
						move = false;
					}
					if (!prevKeyboardState[SDL_SCANCODE_A] && keyboardState[SDL_SCANCODE_A]) {
						// Key A released
						std::cout << "Key A released\n";
						move = false;
					}
					if (!prevKeyboardState[SDL_SCANCODE_D] && keyboardState[SDL_SCANCODE_D]) {
						// Key D released
						std::cout << "Key D released\n";
						move = false;
					}
				}
				

				if (keyboardState[SDL_SCANCODE_W]) {
					sprite.position.y -= speed * 0.016f;
					move = true;
				}
				if (keyboardState[SDL_SCANCODE_S]) {
					sprite.position.y += speed * 0.016f;
					move = true;
				}
				if (keyboardState[SDL_SCANCODE_A]) {
					sprite.position.x -= speed * 0.016f;
					move = true;
				}
				if (keyboardState[SDL_SCANCODE_D]) {
					sprite.position.x += speed * 0.016f;
					move = true;
				}

			

				sprite.rotation = -180;
				
				if(!move) sprite.position.y += 0.55f * sinf(time); // Adjust the multiplier as needed
				sprite.model = glm::mat4(1.0f);
				sprite.model = glm::translate(sprite.model, glm::vec3(sprite.position, 0.0f));
				sprite.model = glm::rotate(sprite.model, glm::radians(sprite.rotation), glm::vec3(0.0, 0.0, 1.0f));
				if(sprite.scale.x > 0) sprite.scale.x *= -1;
				sprite.model = glm::scale(sprite.model, glm::vec3(sprite.scale, 1.0f));

				prevKeyboardState = keyboardState;
			}
		}
		//camera.zoom += 0.005f * sinf(SDL_GetTicks() * 0.001f);


		// Rendering code goes here
		Engine::set_clear_color({ 0, 128, 0, 255 });
		glClear(GL_COLOR_BUFFER_BIT);

        //rotationAngle += 1.0f;
		
		
	
		for (const auto& sprite: sprites) {
			// Pass matrices to the shader
			shader.use();
			shader.set_mat4("projection", projection);
			shader.set_mat4("view", view);
			shader.set_mat4("model", sprite.model);

			glBindTexture(GL_TEXTURE_2D, sprite.texture);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}


		SDL_GL_SwapWindow(window);
	}
	return (0);
}