#pragma once

#include "Scene.h"

#include "TransformComponent.h"
#include "OrthographicCameraComponent.h"

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
		
		IceEngine::Shader *m_shader;
		IceEngine::OrthographicCameraComponent *m_cameraComponent;
		IceEngine::TransformComponent *m_transformComponent;

		uint32_t m_floorTileTexture;
		uint32_t m_wallTileTexture;
		std::vector<glm::vec2> m_textureCoords;
		glm::vec2 prevMousePos;
		
	};
}