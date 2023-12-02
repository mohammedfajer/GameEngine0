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
		void Render() override;
	private:
		IceEngine::SpriteRendererSystem *m_spriteRendererSystem;
		IceEngine::Shader* spriteShader;
		IceEngine::GameObject* CameraGameObject;
	};
}

