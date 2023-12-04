#pragma once

#include "Scene.h"
//#include "SpriteBatch.h"
#include "Shader.h"
#include "OrthographicCameraComponent.h"
#include "TransformComponent.h"

namespace TopDownShooter
{
	class SpriteBatchScene : public IceEngine::Scene
	{
	public:
		SpriteBatchScene();
		~SpriteBatchScene();
		void Update(float deltaTime) override;
		void Render() override;
	private:
		//IceEngine::SpriteBatch* m_spriteBatch;

		IceEngine::Shader* m_shader;
		IceEngine::OrthographicCameraComponent* m_cameraComponent;
		IceEngine::TransformComponent* m_transformComponent;

		uint32_t m_coinTextureId;
		uint32_t m_playerTextureId;
	};
}

