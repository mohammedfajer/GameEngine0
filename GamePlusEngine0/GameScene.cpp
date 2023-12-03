#include "GameScene.h"
#include "Random.h"

#include "Defines.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "InputManager.h"
#include "SpriteRendererSystem.h"
#include "OrthographicCameraComponent.h"

#include "Timer.h"
#include "FileIO.h"
#include "Logger.h"

#include <string>
#include <ctype.h> 

namespace TopDownShooter 
{
	enum class TileType : uint8_t 
	{
		FLOOR 
	};

	struct Tile 
	{
		glm::vec2 position;
		glm::vec2 scale;
		TileType  type;
	};

	struct TileFile
	{
		std::vector<Tile> tiles;
		int rows;
		int columns;
	};

	static TileFile LoadTileMap(const char *filePath)
	{
		TileFile tileFile;

		std::string map = std::string(IceEngine::LoadTextFile(filePath));

		if (map.size() > 0)
		{
			IceEngine::Texture2D tileTexture = IceEngine::TextureLoader::LoadTexture("./data/Tile2.png");
			int width = tileTexture.width;
			int height = tileTexture.height;

			std::string firstLine;
			int startIdx = 0;
			
			while (map[startIdx] != '\n')
			{
				char c = map[startIdx];
				firstLine += c;
				startIdx++;
			}
			
			const char* str = firstLine.c_str();
			sscanf_s(str, "%d %d", &tileFile.rows, &tileFile.columns);

			IceEngine::Logger::Instance().Log("Rows = " + std::to_string(tileFile.rows) + " Columns = " + std::to_string(tileFile.columns));

			int x = 0;
			int y = 0;

			for (int i = startIdx + 1; i < map.size(); i++)
			{
				char c = map[i];

				if (c == ',') continue;
				if (c == '\n') 
				{
					y += height;
					x = 0;
				}
				else 
				{
					Tile t = { glm::vec2(x, y) };
					if (c == '0') t.type = TileType::FLOOR;

					switch (t.type)
					{
						case TileType::FLOOR:
						{
							t.scale = glm::vec2(tileTexture.width, tileTexture.height);
							break;
						}
					}

					tileFile.tiles.push_back(t);
					x += width;
				}
			}
		}
		
		return tileFile;
	}

	void DebugPrintTiles(TileFile& tiles)
	{
		assert(tiles.rows * tiles.columns == tiles.tiles.size());

		for (int y = 0; y < tiles.rows; y++)
		{
			for (int x = 0; x < tiles.columns; x++)
			{
				Tile t = tiles.tiles[(y * tiles.columns) + x];
				IceEngine::Logger::Instance().Log(std::to_string(t.position.x) + " " + std::to_string(t.position.y));
			}
			IceEngine::Logger::Instance().Log("");
		}
	}

	GameScene::GameScene()
	{
		m_name = "Game";

		auto tiles = LoadTileMap("./Map0.txt");

		DebugPrintTiles(tiles);
		
		// Create the game object and add components 👽
		for (int i = 0; i < tiles.rows * tiles.columns; i++)
		{
			IceEngine::GameObject *tileGameObject = new IceEngine::GameObject();
			tileGameObject->SetName("Tile");
			glm::vec2 position = tiles.tiles[i].position + glm::vec2(tiles.tiles[i].scale.x / 2, tiles.tiles[i].scale.y / 2);
			tileGameObject->AddComponent<IceEngine::TransformComponent>(position, tiles.tiles[i].scale, 0.0f);
			tileGameObject->AddComponent<IceEngine::SpriteRendererComponent>("./data/Tile2.png");

			AddGameObject(tileGameObject);
		}

		// Create and set up the SpriteRendererSystem
		m_spriteRendererSystem = new IceEngine::SpriteRendererSystem();

		// Assuming you have a shader ready, set it in the SpriteRendererSystem
		spriteShader = new IceEngine::Shader("vertex_shader.glsl", "fragment_shader.glsl");

		CameraGameObject = new IceEngine::GameObject();
		CameraGameObject->SetName("Camera");
		CameraGameObject->AddComponent<IceEngine::TransformComponent>(glm::vec2(0.0f, 0.0f), glm::vec2(50,50), 0.0f);
		CameraGameObject->AddComponent<IceEngine::OrthographicCameraComponent>(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
		AddGameObject(CameraGameObject);

		m_spriteRendererSystem->SetShader(spriteShader);

		// Game Objects
		std::vector<glm::vec2> coinPositions;

		// Max at 4000 with 11 FPS
		// 
		// Coins
		for (int i = 0; i < 100; i++)
		{
			glm::vec2 randomPosition = IceEngine::GetRandomPosition(0.0f, SCREEN_WIDTH , 0.0f, SCREEN_HEIGHT, 5.0f, coinPositions);
			coinPositions.push_back(randomPosition);
		}

		for (int i = 0; i < 100; i++)
		{
			IceEngine::GameObject* object = new IceEngine::GameObject();
			object->SetName("Coin");
			object->AddComponent<IceEngine::TransformComponent>(coinPositions[i], glm::vec2(50.0f, 50.0f), 0.0f);
			object->AddComponent<IceEngine::SpriteRendererComponent>("./data/coin.png");
			AddGameObject(object);
		}

		coinPositions.clear();
		
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

		// Calculate FPS
		//float FPS = 1.0f / deltaTime;

		//IceEngine::Logger::Instance().Log("FPS : " + std::to_string(FPS), IceEngine::LogLevel::INFO);

		IceEngine::GameObject* player = GetGameObjectByName("Player");

		auto CameraComponent = CameraGameObject->GetComponent<IceEngine::OrthographicCameraComponent>();

		if (player)
		{
			int Speed = 100;
			IceEngine::TransformComponent* transform = player->GetComponent<IceEngine::TransformComponent>();

			// Keyboard
			if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_W) || IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_UP))
			{
				transform->position.y -= Speed * deltaTime;
			}

			if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_S) || IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_DOWN))
			{
				transform->position.y += Speed * deltaTime;
			}

			if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_A) || IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_LEFT))
			{
				transform->position.x -= Speed * deltaTime;
			}

			if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_D) || IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_RIGHT))
			{
				transform->position.x += Speed * deltaTime;
			}

			// Gamepad
			SDL_GameController* gameController = NULL;
			for (int i = 0; i < SDL_NumJoysticks(); i++) 
			{
				if (SDL_IsGameController(i)) 
				{
					gameController = SDL_GameControllerOpen(i);
					if (gameController) 
					{
						break;
					}
				}
			}

			if (gameController)
			{
				if (IceEngine::InputManager::Instance().IsGamepadButtonDown(SDL_CONTROLLER_BUTTON_A))
				{
					transform->position.y += Speed * deltaTime;
				}

				if (IceEngine::InputManager::Instance().IsGamepadButtonDown(SDL_CONTROLLER_BUTTON_X))
				{
					transform->position.x -= Speed * deltaTime;
				}

				if (IceEngine::InputManager::Instance().IsGamepadButtonDown(SDL_CONTROLLER_BUTTON_B))
				{
					transform->position.x += Speed * deltaTime;
				}

				if (IceEngine::InputManager::Instance().IsGamepadButtonDown(SDL_CONTROLLER_BUTTON_Y))
				{
					transform->position.y -= Speed * deltaTime;
				}
			}
			
			
		
			if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_L))
			{
				CameraComponent->zoom += 0.05f;
			}
			if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_K))
			{
				CameraComponent->zoom -= 0.05f;
			}
			if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_P))
			{
				CameraComponent->zoom = 1.0f;
			}
			
			CameraComponent->SetFollowPosition(transform->position + transform->scale * 0.5f, glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
		}



	}

	void GameScene::Render()
	{
		// Render game objects using the SpriteRendererSystem
		for (auto& gameObject : m_gameObjects)
		{
			spriteShader->Bind();

			spriteShader->SetMat4("projection", CameraGameObject->GetComponent<IceEngine::OrthographicCameraComponent>()->projection);
			spriteShader->SetMat4("view", CameraGameObject->GetComponent<IceEngine::OrthographicCameraComponent>()->GetViewMatrix());

			m_spriteRendererSystem->RenderGameObject(*gameObject);
		}
	}
}

