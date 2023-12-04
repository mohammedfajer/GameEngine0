#include "SpriteBatchScene.h"

#include "Renderer.h"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TextureLoader.h"
#include "Color.h"
#include "InputManager.h"
#include "Defines.h"
#include "Logger.h"

namespace TopDownShooter
{

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
		uniform mat4 model;
    
		void main()
		{
			gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
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

		void main()
		{
			int index = int(TexIdx);
			FragColor = texture(u_Textures[index], TexCoord) * OurColor;
			
		}
	)";

	static void SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix)
	{
		int loc = glGetUniformLocation(shader, name);
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	SpriteBatchScene::SpriteBatchScene()
	{
		m_name = "SpriteBatchScene";

		//m_spriteBatch = new IceEngine::SpriteBatch();
		//m_spriteBatch->Begin();


		// Load Texture 1
		IceEngine::Texture2D coinTexture = IceEngine::TextureLoader::LoadTexture("./data/coin.png");
		m_coinTextureId = coinTexture.id;

		// Load Texture 2
		IceEngine::Texture2D playerTexture = IceEngine::TextureLoader::LoadTexture("./data/player.png");
		m_playerTextureId = playerTexture.id;

		// Setup Shader

		m_shader = new IceEngine::Shader();
		m_shader->LoadShaderFromString(vertex_shader.c_str(), fragment_shader.c_str());

		m_shader->Bind();
		auto loc = glGetUniformLocation(m_shader->id, "u_Textures");
		int samplers[32];
		for (int i = 0; i < 32; i++)
		{
			samplers[i] = i;
		}
		glUniform1iv(loc, 32, samplers);

		IceEngine::Renderer::Init();

		m_cameraComponent = new IceEngine::OrthographicCameraComponent(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	

		m_transformComponent = new IceEngine::TransformComponent(glm::vec2(0, 0), glm::vec2(100, 100), 0.0f);

		m_cameraComponent->SetFollowPosition(m_transformComponent->position, glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));

		//m_cameraComponent->SetFollowPosition(glm::vec2(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
		
		

	}
	SpriteBatchScene::~SpriteBatchScene()
	{
		IceEngine::Renderer::Shutdown();
	}


	void SpriteBatchScene::Update(float deltaTime)
	{
		// Update the base scene
		Scene::Update(deltaTime);

		//m_spriteBatch->Update(deltaTime);

		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_L))
		{
			m_cameraComponent->zoom += 0.05f;

		}
		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_K))
		{
			m_cameraComponent->zoom -= 0.05f;
		}
		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_P))
		{
			m_cameraComponent->zoom = 1.0f;
		}
	}

	void SpriteBatchScene::Render()
	{
		//m_spriteBatch->Render();

		IceEngine::Color::SetClearColor({ 1, 71, 94 , 255 });
		glClear(GL_COLOR_BUFFER_BIT);

		m_shader->Bind();

		// OLD
		// 4 K 11 FPS

		/// 1600% FASTER
		// 100 K 72 FPS
		// 650 k 15 FPS

		
		IceEngine::Renderer::ResetStats();
		IceEngine::Renderer::BeginBatch();

		for (float y = -12.5f; y < 12.5f; y += 0.25f)
		{
			for (float x = -125; x < 125.0f; x += 0.25f)
			{
				glm::vec4 color = { (x + 10) / 20.0f, 0.2f, (y + 10) / 20.f, 1.0f };
				IceEngine::Renderer::DrawQuad({ x,y }, { 0.25f, 0.25f }, color);
			}
		}

		for (int y = 0; y < 5; y++)
		{
			for (int x = 0; x < 5; x++)
			{
				GLuint tex = (x + y) % 2 == 0 ? m_coinTextureId : m_playerTextureId;
				IceEngine::Renderer::DrawQuad({ x,y }, { 1.0f, 1.0f }, tex);
			}
		}
		
		auto S = IceEngine::Renderer::GetStats();
	
		IceEngine::Logger::Instance().Log(IceEngine::LogLevel::SUCCESS, "DrawCount = %, QuadCount = %", S.DrawCount, S.QuadCount);
		IceEngine::Renderer::EndBatch();

		// Setup Camera View Transform
		m_shader->SetMat4("projection", m_cameraComponent->projection);
		m_shader->SetMat4("view", m_cameraComponent->GetViewMatrix());
		m_shader->SetMat4("model", m_transformComponent->GetModelMatrix());




		
	}
}