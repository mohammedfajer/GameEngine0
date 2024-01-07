#include "Menu.h"
#include <GL/glew.h>
#include "Defines.h"
#include <SDL.h>
#include "graphics.h"
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

#include "Engine.h"

#include <cassert>
#include <cmath>
#include "core.h"
#include "math.h"
#include "effects.h"
#include "time.h"

namespace TopDownShooter {
	// Scene Transition
	// Menu Image
	// Menu Options
	// Music and Sound Effects
    
	const char *scene_transition_vertex_shader_source_code = R"(
		#version 330 core

		layout(location = 0) in vec3 inVertexPosition;
		layout(location = 1) in vec2 inTexCoord;

		out vec2 fragTexCoord;

		void main() {
			gl_Position = vec4(inVertexPosition, 1.0);
			fragTexCoord = inTexCoord;
		}
	)";

	const char *scene_transition_fragment_shader_source_code = R"(
		#version 330 core
		in vec2 fragTexCoord;

		uniform float cutoff;
		uniform float smooth_size;
		uniform sampler2D mask;
		uniform bool u_switch_animation;

		out vec4 FragColor;

		void main() {
			float value = texture(mask, fragTexCoord).r;
			float alpha;
			if(u_switch_animation) {
				alpha = smoothstep(cutoff, cutoff + smooth_size, value);
			} else {
				alpha = smoothstep(1 - value, 1- value + smooth_size, cutoff);
			}
			FragColor = vec4(vec3(0.0), alpha);  // Set color to black, adjust as needed
		}
	)";

	static IceEngine::Clock myClock;

	static IceEngine::Transition_Effect effect;
	static float duration = 5.0f;

	static IceEngine::Transition_Manager transition_manager(duration);
	static IceEngine::Timer transition_timer;

	static IceEngine::Text menuText;
	

	static IceEngine::OrthographicCameraComponent *camera;
	static int indexSelected = 0;
    
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


	glm::vec2 offset = { SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT - 300 };
	IceEngine::FPS_Counter fps_counter;

    
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
		void main() {
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
    
	

	



	MenuScene::MenuScene() {
		camera = new IceEngine::OrthographicCameraComponent(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
		camera->zoom = 2.0F;
		camera->position = { 24.2, 57.2 };


		IceEngine::Texture2D mask_texture;
		
		mask_texture = IceEngine::TextureLoader::LoadTexture("./data/from_center.png");
		effect.texture_id = mask_texture.id;

		IceEngine::init_transition_effect(&effect, scene_transition_vertex_shader_source_code,
			scene_transition_fragment_shader_source_code);

		
		IceEngine::set_mask_texture(&effect);

		effect.shader.Bind();
		effect.shader.SetFloat("smooth_size", 0.2f);
		effect.shader.UnBind();


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

		transition_timer.set_time(duration);
	}
    
	MenuScene::~MenuScene() {}
    
	bool startGame = false;

	void MenuScene::Update(float deltaTime) {

		myClock.tick();

		// Update the base scene
		Scene::Update(deltaTime);
		
		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_RETURN)) {
			if (indexSelected == 0) {
				if (transition_timer.isExpired()) {
					IceEngine::SceneManager::Instance().SetActiveScene("SpriteSheetScene");
				}
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

		transition_manager.update(&effect, myClock.delta);
	}
	

	void MenuScene::Render() {
		
		IceEngine::Color::SetClearColor({ 72, 59, 58, 255 });
		glClear(GL_COLOR_BUFFER_BIT);

		static float fakeElapsedTime = 0.0f; // Initialize fake time
		float interpolateDuration = 8.0f;    // one second
        
		// Manually update the fake time
		// Assuming 60 frames per second (you can adjust this value based on your frame rate)
		fakeElapsedTime += 0.016f; 
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
	 
		IceEngine::switch_transition_animation(&effect, true);
		IceEngine::set_cutoff(&effect);
		IceEngine::render_transition_effect(&effect);
		fps_counter.update();
		
	}
}
