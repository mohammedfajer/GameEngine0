
#include "AnimationScene.h"
#include "TextureLoader.h"
#include "graphics.h"
#include "Defines.h"
#include "InputManager.h"
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>  // Include this for glm::lerp
#include "DebugDraw.h"
#include "FontSystem.h"
#include "core.h"
#include "Shader.h"
#include "array.h"


// Dear IMGUI
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "core.h"


#include <cstring> // for memset


#define MAX_FRAME_SPEED     15
#define MIN_FRAME_SPEED      1



namespace TopDownShooter {

	static IceEngine::Text info_text;

	static IceEngine::Shader *m_shader;
	static IceEngine::Orthographic_Camera *cam;

	static IceEngine::DebugRect animationTextureRect;
	static std::vector<IceEngine::DebugRect> rects;

	static IceEngine::DebugRect frameRect;
	static IceEngine::DebugRect groundRect;
	
	static IceEngine::Rect frameRec;

	static int currentFrame;
	static int framesCounter = 0;
	static int frameSpeed = 8;

	static int currentFPS = 1;
	static bool flipDir = false;

	//
	// To do: Approxmimately
	//

	static IceEngine::Texture2D animationTexture;

	static const std::string vertex_shader = R"(
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

	static const std::string fragment_shader = R"(
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
	
		
	
    
    static IceEngine::Animation test_animation;
 
	AnimationScene::AnimationScene() {
		
		m_name = "AnimationScene";
		

		cam = new IceEngine::Orthographic_Camera(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0F);
		cam->zoom = 1.0f;
	 
		info_text.font_path = "./data/fonts/ThaleahFat.ttf";
		info_text.fontSize = 28;
		info_text.setup();
		info_text.projection = cam->projection;

		for (int i = 0; i < MAX_FRAME_SPEED; i++) {
			IceEngine::DebugRect r;
			r.setup();
			r.isOutline = true;
			rects.push_back(r);
		}
		
		rects[0].isOutline = false;

		animationTextureRect.setup();
		animationTextureRect.isOutline = true;

		frameRect.setup();
		frameRect.isOutline = true;

		groundRect.setup();
		groundRect.isOutline = false;
		
		animationTexture = IceEngine::TextureLoader::LoadTexture("./data/scarfy.png");

		frameRec = { 0.0f, 0.0f, (float)animationTexture.width / 6, (float)animationTexture.height };
		
		
	    test_animation.name = "Wizzard Female";
	    IceEngine::setup_animation(&test_animation, "./data/Asset0/wizzard_f.png", 5, 3);	
		
        // memset(&test_animation, 0, sizeof(IceEngine::Animation));
        // test_animation.texture = IceEngine::TextureLoader::LoadTexture("./data/Asset0/wizzard_m.png");;
        // test_animation.frame_rect = {0.0f, 0.0f, (float)test_animation.texture.width / 9, (float)test_animation.texture.height};
        // test_animation.looping = true;
        // test_animation.flip = false;
        // test_animation.fps = 7;
        // test_animation.name = "test animation";
        // test_animation.frame_size = vec2(test_animation.frame_rect.w, test_animation.frame_rect.h);
        // test_animation.num_frames = 3;
		
        		

		m_shader = new IceEngine::Shader();
		m_shader->LoadShaderFromString(vertex_shader.c_str(), fragment_shader.c_str());

		m_shader->Bind();
		auto loc = glGetUniformLocation(m_shader->id, "u_Textures");
		int samplers[32];
		for (int i = 0; i < 32; i++) {
			samplers[i] = i;
		}
		glUniform1iv(loc, 32, samplers);

		IceEngine::Renderer::Init();

		// Set sRGB as the framebuffer's color format
		glEnable(GL_FRAMEBUFFER_SRGB);	
	}
	
	AnimationScene::~AnimationScene() {
		glDisable(GL_FRAMEBUFFER_SRGB);
	}

	

	
	
	void AnimationScene::Update(float deltaTime) {
		// Update the base scene
		Scene::Update(deltaTime);

		vec2 mouse_position = { 0,0 };
		mouse_position = IceEngine::InputManager::Instance().GetMousePosition();
		
		// Have ImGUI on editor mode to print the mouse position.
		if (IceEngine::Core::app_mode == IceEngine::App_Mode::Editor) {
			ImGui::Text("Mouse Position %f %f", mouse_position.x, mouse_position.y);
			IceEngine::Logger::Instance().Log(IceEngine::LogLevel::SUCCESS, "Mouse Pos % %",
																				mouse_position.x, mouse_position.y);
		}
		
		framesCounter++;
		if (framesCounter >= (120 / currentFPS)) {
			framesCounter = 0;
			currentFrame++;
			if (currentFrame > 5) currentFrame = 0;
			frameRec.x = (float) currentFrame *(float)128;
		 }
		
		 IceEngine::update_animation(&test_animation);

		/*	if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_RIGHT)) {
				currentFPS += 1;
				currentFPS %= MAX_FRAME_SPEED + 1;
				if (currentFPS == 0) currentFPS = 1;
			}

			if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_LEFT)) {
				currentFPS -= 1;
				currentFPS = (currentFPS % MAX_FRAME_SPEED + MAX_FRAME_SPEED) % MAX_FRAME_SPEED;
				if (currentFPS <= 0) currentFPS = 1;
			}*/


		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_RIGHT)) currentFPS++;
		else if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_LEFT)) currentFPS--;

		if (currentFPS > MAX_FRAME_SPEED) currentFPS = MAX_FRAME_SPEED;
		else if (currentFPS < MIN_FRAME_SPEED) currentFPS = MIN_FRAME_SPEED;

		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_F)) flipDir = !flipDir;		
	}
	
	void AnimationScene::Render() {
		IceEngine::Color::SetClearColor({ 74, 58, 27 , 245 });
		glClear(GL_COLOR_BUFFER_BIT);

		m_shader->Bind();

		// Setup Camera View Transform
		m_shader->SetMat4("projection", cam->projection);
		m_shader->SetMat4("view", cam->view_matrix());

		glEnable(GL_FRAMEBUFFER_SRGB);
		
		IceEngine::Renderer::BeginBatch();
		IceEngine::Renderer::DrawQuad(vec2(515, 115), vec2(768, 128), animationTexture.id);

		// Textured Quad (Player)
		{
		    auto textureCoord = IceEngine::GetTextureCoords(animationTexture, frameRec.y, frameRec.x, frameRec.w, frameRec.h);
			vec2 frameSize    = { (float)frameRec.w, (float)frameRec.h };
			vec2 position     = vec2(950, 600);
			
			// Translation matrix with the pivot adjustment
			mat4 translation    = glm::translate(mat4(1.0f), vec3(position.x , position.y, 0.0f));
			mat4 rotationMatrix = glm::rotate(mat4(1.0f), glm::radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
			mat4 scaling        = glm::scale(mat4(1.0f), vec3(frameSize.x, -frameSize.y, 1.0f));

			if (flipDir) {
				scaling = glm::scale(mat4(1.0f), vec3(-frameSize.x, -frameSize.y, 1.0f));
			}
 
			mat4 model = translation * rotationMatrix * scaling;
			frameSize  = { (float)frameRec.w / SCREEN_WIDTH, (float)frameRec.h / SCREEN_HEIGHT };

			IceEngine::Renderer::DrawQuad(position, frameSize, model, animationTexture.id, textureCoord);
		}

		IceEngine::render_animation(&test_animation, vec2(1200, 600), 0.0f, 3.0f);

		IceEngine::Renderer::EndBatch();
		IceEngine::Renderer::Flush();

		m_shader->UnBind();

		glDisable(GL_FRAMEBUFFER_SRGB);

		// Compute the center point starting at p.
		vec2 p = {515, 115};
		p = p + 0.5f * vec2(768, 128);
	 
		animationTextureRect.draw(p.x, p.y, 768, 128, cam->view_matrix(), cam->projection, vec3(0,255,0));

		// Compute the center point for the rect frame.
		vec2 recPos = {515, 115};
		recPos = recPos + 0.5f * vec2(128,128);
		
		frameRect.draw(recPos.x + (int)frameRec.x, recPos.y, 128, 128, cam->view_matrix(), cam->projection, vec3(255, 0, 0));
		
		info_text.draw("FRAME SPEED:", 300, 310, 1.0f, vec3(123, 123, 0));

		for (int i = 0; i < rects.size(); i++) {
			int x = 520 + 55 * i;
			int y = 300;
			if (i < currentFPS) rects[i].isOutline = false;
			else rects[i].isOutline = true;
			rects[i].draw(x, y, 50, 50, cam->view_matrix(), cam->projection);
		}

		std::string fpsText;
		if (currentFPS < 10) {
			fpsText += "0";
			fpsText += std::to_string(currentFPS);
		}
		else {
			fpsText += std::to_string(currentFPS);
		}

		info_text.draw( fpsText + " FPS", 1355, 310, 1.0f, vec3(123, 123, 0));
		info_text.draw("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 650, 390, 1.0f, vec3(123, 123, 0));	

		// Ground Rect
		vec2 position = {200,650};
		vec2 size = {1500, 50};
		position = position + 0.5f * size;
		groundRect.draw(position.x, position.y, size.x, size.y, cam->view_matrix(), cam->projection, vec3(255, 221, 128));
	}
}
