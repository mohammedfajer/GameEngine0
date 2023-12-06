#pragma once

#include "Scene.h"
#include "Shader.h"
#include "OrthographicCameraComponent.h"
#include "TransformComponent.h"

namespace TopDownShooter
{
	class ParticlesScene : public IceEngine::Scene
	{
	public:
		ParticlesScene();
		~ParticlesScene();
		void Update(float deltaTime) override;
		void Render() override;
	private:
		IceEngine::Shader* m_shader;
		IceEngine::OrthographicCameraComponent* m_cameraComponent;
		IceEngine::TransformComponent* m_transformComponent;
	};
}