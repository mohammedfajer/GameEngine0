
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <cmath>

#include "Logger.h"
#include "Engine.h"
#include "Timer.h"
#include "Shader.h"
#include "Sprite.h"
#include "OrthographicCamera.h"
#include "Texture2D.h"
#include "Color.h"



int main(int argc, char *argv[]) {

	IceEngine::Engine::GetInstance().Start();

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
    IceEngine::Shader shader(vertex_shader, fragment_shader);

	// Main Loop
	bool quit = false;

    // Camera 
	IceEngine::OrthographicCamera camera (0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	camera.position = glm::vec2(0.0f, 0.0f);
	camera.zoom = 1.0f;



	std::vector<IceEngine::Sprite> sprites;
	IceEngine::Texture2D coinTexture = IceEngine::load_texture("./data/coin.png");
	IceEngine::Texture2D backgroundTexture = IceEngine::load_texture("./data/bg.png");

	for (int i = 0; i < 25; ++i) {
		glm::vec2 randomPosition = IceEngine::generateRandomPosition(sprites, 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, 50.0f);
		sprites.push_back(IceEngine::Sprite(randomPosition, glm::vec2(50.0f, 50.0f), coinTexture.id));
	}

	IceEngine::Texture2D playerTexture = IceEngine::load_texture("./data/plane_1_pink.png");
	sprites.push_back(IceEngine::Sprite(glm::vec2(300, 350), glm::vec2(50, 50), playerTexture.id, "player"));



	/*
	
		Engine::Run
			Dt = Time::Instance()->Tick()l
			m_window->Update();
			InputManager::Instance()->Update();
			SceneManager::Instance()->UpdateCurrentScene(dt);
			Application::Instance()->Update(dt);
			CollisionManager::Instance()->Update(dt);
			AfterUpdate();
			Render();

	*/


	while (!quit) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) quit = true;
		}


		float deltaTime = IceEngine::Timer::Instance().Tick();

		// calculate FPS
		float FPS = 1.0f / deltaTime;

		IceEngine::Logger::GetInstance().Log("FPS : " + std::to_string(FPS), IceEngine::LogLevel::INFO); 

		// Get the current state of the keyboard
		const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

		glm::mat4 projection = camera.projection;
		glm::mat4 view = camera.get_view_matrix();

		for (IceEngine::Sprite &sprite: sprites) {
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
						 move = false;
					}
					if (!prevKeyboardState[SDL_SCANCODE_S] && keyboardState[SDL_SCANCODE_S]) {
						// Key S released
						move = false;
					}
					if (!prevKeyboardState[SDL_SCANCODE_A] && keyboardState[SDL_SCANCODE_A]) {
						// Key A released
						move = false;
					}
					if (!prevKeyboardState[SDL_SCANCODE_D] && keyboardState[SDL_SCANCODE_D]) {
						// Key D released
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
		IceEngine::set_clear_color({ 0, 128, 0, 255 });
		glClear(GL_COLOR_BUFFER_BIT);

      

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


		SDL_GL_SwapWindow(IceEngine::Engine::GetInstance().GetWindow());
	}

	IceEngine::Engine::GetInstance().Shutdown();

	return (0);
}