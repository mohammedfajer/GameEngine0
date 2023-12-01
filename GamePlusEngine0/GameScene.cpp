#include "GameScene.h"
#include "Random.h"

#include "Defines.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"

#include "SpriteRendererSystem.h"
#include "OrthographicCameraComponent.h"

namespace TopDownShooter 
{
	GameScene::GameScene()
	{
		m_name = "Game";

		// Create and set up the SpriteRendererSystem
		m_spriteRendererSystem = new IceEngine::SpriteRendererSystem();

		// Assuming you have a shader ready, set it in the SpriteRendererSystem
		IceEngine::Shader* spriteShader = new IceEngine::Shader("vertex_shader.glsl", "fragment_shader.glsl");
		m_spriteRendererSystem->SetShader(spriteShader);

		IceEngine::GameObject* object = new IceEngine::GameObject();
		object->SetName("Camera");
		object->AddComponent<IceEngine::TransformComponent>()->position = glm::vec2(0.0f, 0.0f);
		object->AddComponent<IceEngine::OrthographicCameraComponent>(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f)->zoom = 1.0f;
		AddGameObject(object);
		

		// Game Objects
		
		// Coins
		for (int i = 0; i < 25; i++) 
		{
			glm::vec2 randomPosition = IceEngine::GetRandomPosition(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, 50.0f);
			IceEngine::GameObject* object = new IceEngine::GameObject();
			object->SetName("Coin");
			object->AddComponent<IceEngine::TransformComponent>()->position = randomPosition;
			object->AddComponent<IceEngine::SpriteRendererComponent>("./data/coin.png");
			AddGameObject(object);
		}

		// Player
		IceEngine::GameObject* object1 = new IceEngine::GameObject();
		object1->SetName("Player");
		auto playerTransform = object1->AddComponent<IceEngine::TransformComponent>();
		playerTransform->owner		= object1;
		playerTransform->position	= glm::vec2(300, 350);
		playerTransform->scale		= glm::vec2(50, 50);
		object1->AddComponent<IceEngine::SpriteRendererComponent>("./data/plane_1_pink.png");
		AddGameObject(object1);
	}

	GameScene::~GameScene()
	{
		// Clean up resources
		delete m_spriteRendererSystem;
	}


	// Override the Update function if needed
	void GameScene::Update(float deltaTime) 
	{
		// Update the base scene
		Scene::Update(deltaTime);

		// Additional updates specific to the GameScene if needed

		// Render game objects using the SpriteRendererSystem
		for (auto& gameObject : m_gameObjects)
		{
			m_spriteRendererSystem->RenderGameObject(*gameObject);
		}
	}

}

