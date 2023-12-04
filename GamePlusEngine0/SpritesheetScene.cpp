#include "SpritesheetScene.h"

#include "TextureLoader.h"
#include "Color.h"

#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "OrthographicCameraComponent.h"
#include "Defines.h"

namespace TopDownShooter
{
	float ConvertToUV(float coord, int size)
	{
		return coord / static_cast<float>(size);  // Adjust for 0-based indexing
	}

	SpriteSheetScene::SpriteSheetScene()
	{

		// 1. load sprite sheet texture
		IceEngine::Texture2D spritesheetTexture = IceEngine::TextureLoader::LoadTexture("./data/RPGpack_sheet.png");

		// 2. get basic information 
		int textureWidth = spritesheetTexture.width;
		int textureHeight = spritesheetTexture.height;
		int tileWidth = 64;
		int tileHeight = 64;

		IceEngine::Logger::Instance().Log(IceEngine::LogLevel::INFO,
         "Texture width = %, height = %, tile width = %, tile height = %", textureWidth, textureHeight, tileWidth, tileHeight);

		/*
			std::vector<Vertex> vertices = {
				{ { +0.5f, +0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },  // top right
				{ { +0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },  // bottom right
				{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  // bottom left
				{ { -0.5f, +0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } }   // top left
			};

		*/

		// (top left)    0,0		64,0 (top right)
		//
		//
		// (bottom left) 0,64     64,64 (bottom right)

		// Convert to 0,0
		// x/width  (0,0)		64/1280 0.05  0
		// y/height


		glm::vec2 topLeft = glm::vec2(ConvertToUV(0, textureWidth), ConvertToUV(0, textureHeight));
		glm::vec2 topRight = glm::vec2(ConvertToUV(64, textureWidth), ConvertToUV(0, textureHeight));
		glm::vec2 bottomLeft = glm::vec2(ConvertToUV(0, textureWidth), ConvertToUV(64, textureHeight));
		glm::vec2 bottomRight = glm::vec2(ConvertToUV(64, textureWidth), ConvertToUV(64, textureHeight));


		// 6. Load shader and sprite renderer system
		spriteShader = new IceEngine::Shader("vertex_shader.glsl", "fragment_shader.glsl");

		std::vector<glm::vec2> customTextureCoords = {
			topRight, bottomRight, bottomLeft, topLeft
		};

		
		m_spriteRendererSystem = new IceEngine::SpriteRendererSystem(customTextureCoords);
		m_spriteRendererSystem->SetShader(spriteShader);

		// Update the vertices texture coordinates for the shader geometry

		// 3. Create a game object

		IceEngine::GameObject* object = new IceEngine::GameObject();
		object->SetName("SpriteSheet");


		// 4. Attach transform component and sprite renderer component
		object->AddComponent<IceEngine::TransformComponent>(glm::vec2(100 + tileWidth /2, 100 + tileHeight /2), glm::vec2(tileWidth , tileHeight * -1), 0.0f);
		object->AddComponent<IceEngine::SpriteRendererComponent>("./data/RPGpack_sheet.png");

		// 5. add game object
		AddGameObject(object);

		// Camera
		CameraGameObject = new IceEngine::GameObject();
		CameraGameObject->SetName("Camera");
		CameraGameObject->AddComponent<IceEngine::TransformComponent>(glm::vec2(0.0f, 0.0f), glm::vec2(50, 50), 0.0f);
		CameraGameObject->AddComponent<IceEngine::OrthographicCameraComponent>(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
		AddGameObject(CameraGameObject);
	}
}



TopDownShooter::SpriteSheetScene::~SpriteSheetScene()
{
}

void TopDownShooter::SpriteSheetScene::Update(float deltaTime)
{
}

void TopDownShooter::SpriteSheetScene::Render()
{

	/*IceEngine::Color::SetClearColor({ 107, 79, 7 , 255 });
	glClear(GL_COLOR_BUFFER_BIT);*/

	// Render game objects using the SpriteRendererSystem
	for (auto& gameObject : m_gameObjects)
	{
		spriteShader->Bind();

		spriteShader->SetMat4("projection", CameraGameObject->GetComponent<IceEngine::OrthographicCameraComponent>()->projection);
		spriteShader->SetMat4("view", CameraGameObject->GetComponent<IceEngine::OrthographicCameraComponent>()->GetViewMatrix());

		m_spriteRendererSystem->RenderGameObject(*gameObject);
	}
}
