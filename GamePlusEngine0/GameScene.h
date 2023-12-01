#pragma once

#include "Scene.h"
#include "SpriteRendererSystem.h"

namespace TopDownShooter
{
	class GameScene : public IceEngine::Scene
	{
	public:
		GameScene();
		~GameScene();
		void Update(float deltaTime) override;
	private:
		IceEngine::SpriteRendererSystem *m_spriteRendererSystem;
	};
}

