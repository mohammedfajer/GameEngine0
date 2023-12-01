#include "GameScene.h"
#include "Random.h"

#include "Defines.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"


namespace TopDownShooter 
{
	GameScene::GameScene()
	{
		m_name = "Game";

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
}

