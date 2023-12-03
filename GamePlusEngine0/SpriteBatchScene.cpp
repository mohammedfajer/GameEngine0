#include "SpriteBatchScene.h"

namespace TopDownShooter
{
	SpriteBatchScene::SpriteBatchScene()
	{
		m_name = "SpriteBatchScene";

		m_spriteBatch = new IceEngine::SpriteBatch();
		m_spriteBatch->Begin();


	}
	SpriteBatchScene::~SpriteBatchScene()
	{
	}

	void SpriteBatchScene::Update(float deltaTime)
	{
		// Update the base scene
		Scene::Update(deltaTime);
	}

	void SpriteBatchScene::Render()
	{
		m_spriteBatch->Render();
	}
}