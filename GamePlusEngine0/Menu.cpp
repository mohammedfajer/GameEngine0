#include "Menu.h"
#include <GL/glew.h>
#include "Defines.h"
#include <SDL.h>
#include "Color.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "FontSystem.h"
#include "OrthographicCameraComponent.h"
#include "InputManager.h"
#include "Texture2D.h"
#include "TextureLoader.h"
#include "Renderer.h"
#include "Shader.h"
#include "SceneManager.h"

namespace TopDownShooter {
	// Scene Transition
	// Menu Image
	// Menu Options
	// Music and Sound Effects
    
	const char *scene_transition_vertex_shader_source_code = R"(
		#version 330 core
		layout (location = 0) in vec2 aPos;
		void main() {
			gl_Position = vec4(aPos, 0.0, 1.0);
		}
	)";
    
	const char *scene_transition_fragment_shader_source_code = R"(
		#version 330 core
    out vec4 FragColor;
    uniform float iTime;
    uniform float iDuration;
    uniform vec2 iResolution;
    uniform vec3 ibgColor;
    uniform bool iOpenTransition;

    void main() {
     float transitionDuration = iDuration;
     float progress = smoothstep(0.0, 1.0, mod(iTime, transitionDuration) / transitionDuration);
     float rectanglePositionTop = 1.0 - smoothstep(0.0, 1.0, progress);
     float rectanglePositionBottom = smoothstep(0.0, 1.0, progress);
     float screenHeight = iResolution.y;
     float rectangleYTop;
     float rectangleYBottom;

     if (iOpenTransition) {
         rectangleYTop = mix(0.0, screenHeight * 0.5, rectanglePositionTop);
         rectangleYBottom = mix(screenHeight * 0.5, screenHeight, rectanglePositionBottom);
     } else {
         rectangleYTop = mix(screenHeight * 0.5, 0.0, rectanglePositionTop);
         rectangleYBottom = mix(screenHeight, screenHeight * 0.5, rectanglePositionBottom);
     }
     // Only draw the top and bottom rectangles, discard the rest
     if (gl_FragCoord.y < rectangleYTop || gl_FragCoord.y > rectangleYBottom) {
         discard;
     }
     // Set color to black for the rectangles
     vec3 finalColor = vec3(0.0, 0.0, 0.0);
     FragColor = vec4(finalColor, 1.0);
    }
	)";



	const char *scene_transition_vertex_shader_source_code_two = R"(
		#version 330 core

		layout(location = 0) in vec3 inVertexPosition;
		layout(location = 1) in vec2 inTexCoord;

		out vec2 fragTexCoord;

		void main() {
			gl_Position = vec4(inVertexPosition, 1.0);
			fragTexCoord = inTexCoord;
		}
	)";

	const char *scene_transition_fragment_shader_source_code_two = R"(
		#version 330 core
		in vec2 fragTexCoord;

		uniform float cutoff;
		uniform sampler2D mask;

		out vec4 FragColor;

		void main() {
			float value = texture(mask, fragTexCoord).r;
			float alpha = step(cutoff, value);
			FragColor = vec4(vec3(0.0), alpha);  // Set color to black, adjust as needed
		}
	)";
    
	struct Transition_Effect {
		GLuint VAO, VBO, EBO;
		IceEngine::Shader shader;
	};

	void init_transition_effect(Transition_Effect *effect, const char *vs, const char *fs) {
		effect->shader.LoadShaderFromString(vs, fs);

		// Set up vertex data (assuming a rectangle covering the entire screen)
		GLfloat vertices[] = {
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom-right
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // top-right
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top-left
		};

		GLuint indices[] = {
			0, 1, 2,  // first triangle
			0, 2, 3   // second triangle
		};

		// Vertex Array Object
		glGenVertexArrays(1, &effect->VAO);
		glBindVertexArray(effect->VAO);

		// Vertex Buffer Object
		glGenBuffers(1, &effect->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, effect->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Element Buffer Object
		glGenBuffers(1, &effect->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, effect->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
		glEnableVertexAttribArray(0);

		// Texture coordinate attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// Unbind VAO
		glBindVertexArray(0);

	}

	void set_mask_texture(Transition_Effect *effect, GLuint texture_id) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		GLint boundTextureID;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTextureID);
		std::cout << "Bound Texture ID: " << boundTextureID << std::endl;

		effect->shader.Bind();
		effect->shader.SetInt("mask", 0);
		effect->shader.UnBind();
	}

	void set_cutoff(Transition_Effect *effect, float cutoff) {
		effect->shader.Bind();
		effect->shader.SetFloat("cutoff", cutoff);
		effect->shader.UnBind();
	}

	void render_transition_effect(Transition_Effect *effect) {
		effect->shader.Bind();

		glBindVertexArray(effect->VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		effect->shader.UnBind();
	}

	struct Timer {
		Uint32 startTime;
		Uint32 duration;
		Timer() = default;
		Timer(Uint32 duration) : duration( static_cast<Uint32>(duration * 1000.0f) ) {reset();}
		void reset() { startTime = SDL_GetTicks(); }
		bool isExpired() const {
			Uint32 currentTime = SDL_GetTicks();
			return (currentTime - startTime) >= duration;
		}
	};
    
	struct SceneTransitionEffect {
		IceEngine::Shader shader;

		GLuint VAO, VBO;
        
		float duration = 1.0f;
		Timer timer;
		bool timerStarted = false;
		uint32_t startTime;
		bool isOpenTransition = true;
        
		void setup() {

			//glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			startTime = SDL_GetTicks();
			timer.duration = static_cast<Uint32>(duration * 1000.0f); // to seconds
            
			shader.LoadShaderFromString(scene_transition_vertex_shader_source_code, scene_transition_fragment_shader_source_code);

			float vertices[] = {
				-1.0f, -1.0f,
				1.0f, -1.0f,
				-1.0f, 1.0f,
				1.0f, 1.0f
			};
            
			// Initialize VAO and VBO for a quad
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
            
			glBindVertexArray(VAO);
            
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
			glEnableVertexAttribArray(0);
            
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
            
			shader.Bind();

			// 230, 217, 181
			// 72 59 58
			shader.SetVec3("ibgColor", glm::vec3(230 / 255.0f, 217 / 255.0f, 181 / 255.0f));
			
		}
        
		void stop() { timerStarted = false; }

		void start() { timerStarted = true; timer.reset(); }
		
        
		void draw() {
			if (!timerStarted) return;

			if (timer.isExpired()) return;
            
			shader.Bind();
			
			glBindVertexArray(VAO);

      
			uint32_t currTime = SDL_GetTicks();
            
			double elapsedTime = (currTime - startTime) / 1000.0; // Convert to seconds.
			
			shader.SetInt("iOpenTransition", isOpenTransition);
			shader.SetFloat("iTime", static_cast<float>(elapsedTime * 0.9));
			shader.SetFloat("iDuration", duration);
			shader.SetVec2("iResolution", glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
            
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);

			shader.UnBind();
		}
	};
    
	/*SceneTransitionEffect OpenTransition;
	SceneTransitionEffect CloseTransition;*/


	Transition_Effect effect;


	IceEngine::Text menuText;
	IceEngine::OrthographicCameraComponent *camera;
	int indexSelected = 0;
    
	struct MenuText_Data {
		std::string text;
		glm::vec2 startPoint;
		glm::vec2 endPoint;
		glm::vec3 color;
		bool selected;
	};
    
	std::vector<MenuText_Data> menuTextList;
	IceEngine::Texture2D menuTexture;
	IceEngine::Shader *shader;
    
	const std::string vertex_shader = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec4 aColor;
		layout (location = 2) in vec2 aTexCoord;
		layout (location = 3) in float aTexIdx;

		out vec4 OurColor;
		out vec2 TexCoord;
		out float TexIdx;

		uniform mat4 projection;
		uniform mat4 view;

		void main()
		{
			gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);
			OurColor = aColor;
			TexCoord = aTexCoord;
			TexIdx = aTexIdx;
		}
	)";
    
	const std::string fragment_shader = R"(
		#version 330 core
		out vec4 FragColor;

		in vec4 OurColor;
		in vec2 TexCoord;
		in float TexIdx;

		uniform sampler2D u_Textures[32];

		void main() {
			int index = int(TexIdx);
			FragColor = texture(u_Textures[index], TexCoord) * OurColor;			
		}
	)";
    

	glm::vec2 offset = { SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT - 300 };
	IceEngine::Texture2D mask_texture;


	MenuScene::MenuScene() {
		camera = new IceEngine::OrthographicCameraComponent(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
		camera->zoom = 2.0F;
		camera->position = { 24.2, 57.2 };


		/*
		 OpenTransition.duration = 3.0f;
		 OpenTransition.setup();

		 CloseTransition.duration = 3.0f;
		 CloseTransition.isOpenTransition = false;

		 OpenTransition.start();*/


		 mask_texture = IceEngine::TextureLoader::LoadTexture("./data/curtain.png");

		init_transition_effect(&effect, scene_transition_vertex_shader_source_code_two,
			scene_transition_fragment_shader_source_code_two);

		
		set_mask_texture(&effect, mask_texture.id);


		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			// Handle the error (print error code or description)
			std::cerr << "OpenGL Error: " << error << std::endl;
		}

		
		menuText.font_path = "./data/fonts/ThaleahFat.ttf";
		menuText.fontSize = 48;
		menuText.projection = camera->projection;
		menuText.setup();
        
		// load sprite sheet texture
		menuTexture = IceEngine::TextureLoader::LoadTexture("./data/Asset0/menu_img.png");
        
		shader = new IceEngine::Shader();
		shader->LoadShaderFromString(vertex_shader.c_str(), fragment_shader.c_str());
		shader->Bind();
        
		auto loc = glGetUniformLocation(shader->id, "u_Textures");
		int samplers[32];
		for (int i = 0; i < 32; i++) {
			samplers[i] = i;
		}
		glUniform1iv(loc, 32, samplers);
        
		IceEngine::Renderer::Init();
        
		// Text			Start Position			End Position			Color		Selected or not
		menuTextList.push_back({ { "Play" },
                                   { -500, SCREEN_HEIGHT - 300 },
                                   glm::vec2{ 100, 10 },
                                   glm::vec3(1.0, 0.8f, 0.2f), true });
        
		menuTextList.push_back({ { "Fullscreen OFF" },
                                   { -100, SCREEN_HEIGHT - 250 },
                                   glm::vec2{10, 50 },
                                   glm::vec3(1.0, 0.8f, 0.2f), false });
        
		menuTextList.push_back({ { "Quit" },
                                   { -500, SCREEN_HEIGHT - 200 },
                                   glm::vec2{105, 90},
                                   glm::vec3(1.0, 0.8f, 0.2f), false });
	}
    
	MenuScene::~MenuScene() {}
    
	bool startGame = false;

	void MenuScene::Update(float deltaTime) {
		// Update the base scene
		Scene::Update(deltaTime);
		static bool firstTime = false;
		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_RETURN)) {
			if (indexSelected == 0) {
				/*if (OpenTransition.timer.isExpired() && !CloseTransition.timerStarted) {
					startGame = true;
					CloseTransition.setup();
					CloseTransition.start();
				}*/
			}
		}
        
		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_DOWN)) {
			indexSelected = indexSelected + 1;
			indexSelected %= 3;
		}
        
		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_UP)) {
			indexSelected = indexSelected - 1;
			// Ensure indexSelected is non-negative
			indexSelected = (indexSelected % 3 + 3) % 3;
		}
        
		for (int i = 0; i < menuTextList.size(); i++) {
			if (i == indexSelected) {
				menuTextList[i].selected = true;
			} else {
				menuTextList[i].selected = false;
			}
		}

		/*if (OpenTransition.timer.isExpired()) {
			OpenTransition.stop();
		}

		if (CloseTransition.timer.isExpired() && startGame && !firstTime) {
			CloseTransition.stop();

			firstTime = true;

			CloseTransition.isOpenTransition = true;
			CloseTransition.start();

		}

		if (CloseTransition.timer.isExpired() && startGame && firstTime) {
			CloseTransition.stop();
			IceEngine::SceneManager::Instance().SetActiveScene("SpriteSheetScene");
		}*/

	}
    
	void MenuScene::Render() {
		IceEngine::Color::SetClearColor({ 72, 59, 58, 255 });
		glClear(GL_COLOR_BUFFER_BIT);

		static float fakeElapsedTime = 0.0f; // Initialize fake time
		float interpolateDuration = 8.0f;    // one second
        
		// Manually update the fake time
		// Assuming 60 frames per second (you can adjust this value based on your frame rate)
		//fakeElapsedTime += 0.016f; 
		// Calculate a single interpolation factor for all texts
		float interpolateFactor = std::min(1.0f, fakeElapsedTime / interpolateDuration);
        
		for (auto &textData : menuTextList) {
			if (textData.selected)
				textData.color = glm::vec3(1.0);
			else
				textData.color = glm::vec3(1.0, 0.8f, 0.2f);
            
			// Linear interpolation for x and y positions
			float interpolateX = glm::mix(-1000.0f, offset.x, interpolateFactor);
			float interpolateY = glm::mix(500.0f, offset.y, interpolateFactor);

			interpolateX += textData.endPoint.x;
			interpolateY += textData.endPoint.y;

			menuText.draw(textData.text, interpolateX, interpolateY, 1.0f, textData.color);
		}
		fakeElapsedTime += 0.016f;
		shader->Bind();
		shader->SetMat4("projection", camera->projection);
		shader->SetMat4("view", camera->GetViewMatrix());
        
		IceEngine::Renderer::BeginBatch();
		IceEngine::Renderer::DrawQuad(glm::vec2(SCREEN_WIDTH/4.0f - 1024/7.5f , 50),
                                      glm::vec2(1024*.3, 512 *0.3f), menuTexture.id);
		IceEngine::Renderer::EndBatch();
		IceEngine::Renderer::Flush();


		static float progress = 0.0;


		set_cutoff(&effect, progress);
		set_mask_texture(&effect, mask_texture.id);
		render_transition_effect(&effect);

		progress += 0.002f;


		/*OpenTransition.draw();
		CloseTransition.draw();*/
	}
}
