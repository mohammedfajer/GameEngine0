#pragma once

#include "Scene.h"
#include "SpriteRendererSystem.h"
#include "Shader.h"

namespace TopDownShooter
{
	class SpriteSheetScene : public IceEngine::Scene
	{
	public:
		SpriteSheetScene();
		~SpriteSheetScene();
		void Update(float deltaTime) override;
		void Render() override;
	private:
		IceEngine::SpriteRendererSystem* m_spriteRendererSystem;
		IceEngine::Shader* spriteShader;
		IceEngine::GameObject* CameraGameObject;
	};
}