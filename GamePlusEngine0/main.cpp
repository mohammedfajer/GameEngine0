

#include <SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>


int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
		return (-1);
	}

	SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	if (!window) {
		std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
		return (-1);
	}

	// Create an OpenGL Context
	SDL_GLContext context = SDL_GL_CreateContext(window);

	// Initialize GLEW
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cerr << "GLEW initialization failed: " << glewGetErrorString(glewError) << std::endl;
		return (-1);
	}

	// Check OpenGL Version
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	// Example glm usage
	glm::vec3 vector1(1.0f, 2.0f, 3.0f);
	glm::vec3 vector2(4.0f, 5.0f, 6.0f);
	glm::vec3 result = vector1 + vector1;

	std::cout << "Result of glm vector addition : (" << result.x << ", " << result.y << ", " << result.z << ")\n";

	// Set the clear color to green
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

	// Main Loop
	bool quit = false;
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) quit = true;
		}

		// Rendering code goes here
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
	}
	return (0);
}