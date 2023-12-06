#include "SpritesheetScene.h"

#include "TextureLoader.h"
#include "Color.h"

#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "OrthographicCameraComponent.h"
#include "Defines.h"

#include "SpritsheetLoader.h"
#include "InputManager.h"
#include "Renderer.h"

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

	float ConvertToUV(float coord, int size)
	{
		return coord / static_cast<float>(size);  // Adjust for 0-based indexing
	}

	SpriteSheetScene::SpriteSheetScene()
	{

		// 1. load sprite sheet texture
		IceEngine::Texture2D floorTileTexture = IceEngine::TextureLoader::LoadTexture("./data/tile0.png");
		m_floorTileTexture = floorTileTexture.id;

		
		IceEngine::Texture2D wallTileTexture = IceEngine::TextureLoader::LoadTexture("./data/tile1.png");
		m_wallTileTexture = wallTileTexture.id;
		
	


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
		m_transformComponent = new IceEngine::TransformComponent(glm::vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), glm::vec2(100, 100), 0.0f);
		m_cameraComponent->SetFollowPosition(glm::vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));



		
		int tileX = 64;
		int tileY = 64;
		int tileW = 64;
		int tileH = 64;

		//m_textureCoords = GetTextureCoords(spritesheetTexture, tileX, tileY, tileW, tileH);

		
		prevMousePos = IceEngine::InputManager::Instance().GetMousePosition();
	}
}



TopDownShooter::SpriteSheetScene::~SpriteSheetScene()
{
}

void TopDownShooter::SpriteSheetScene::Update(float deltaTime)
{
	// Update the base scene
	Scene::Update(deltaTime);

	//m_spriteBatch->Update(deltaTime);

	glm::vec2 mousePos = IceEngine::InputManager::Instance().GetMousePosition();


	if (IceEngine::InputManager::Instance().IsMouseButtonDown(SDL_BUTTON_LEFT)) {
		// Calculate the difference in mouse position from the previous frame
		glm::vec2 mouseDelta = mousePos - prevMousePos;

		// Adjust the camera position based on the mouse movement
		m_cameraComponent->position -= mouseDelta;

		// Store the current mouse position for the next frame
		prevMousePos = mousePos;
	}


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

void TopDownShooter::SpriteSheetScene::Render()
{
	IceEngine::Color::SetClearColor({ 0, 0, 0 , 255 });
	glClear(GL_COLOR_BUFFER_BIT);

	m_shader->Bind();



	IceEngine::Renderer::ResetStats();
	IceEngine::Renderer::BeginBatch();

	
	for(float i=-0.5; i < 4; i+=0.5) 
		IceEngine::Renderer::DrawQuad({ i, -0.5 }, { 0.5, 0.5 }, m_wallTileTexture);
	
	for (float i = 0; i < 5; i += 0.5)
	{
		for (float j = 0; j < 5; j += 0.5)
		{
			IceEngine::Renderer::DrawQuad({ i,j }, { 0.5,0.5 }, m_floorTileTexture);
		}
	}

	
	auto S = IceEngine::Renderer::GetStats();

	IceEngine::Logger::Instance().Log(IceEngine::LogLevel::SUCCESS, "DrawCount = %, QuadCount = %", S.DrawCount, S.QuadCount);
	IceEngine::Renderer::EndBatch();

	// Setup Camera View Transform
	m_shader->SetMat4("projection", m_cameraComponent->projection);
	m_shader->SetMat4("view", m_cameraComponent->GetViewMatrix());
	m_shader->SetMat4("model", m_transformComponent->GetModelMatrix());

	IceEngine::Renderer::Flush();
	
}
