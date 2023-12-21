#pragma once

#include "Scene.h"

#include "TransformComponent.h"
#include "OrthographicCameraComponent.h"

#include "Shader.h"
#include "Tilesystem.h"



namespace TopDownShooter
{
	struct Tile_Info {
		std::string name;
		int x_offset;
		int y_offset;
		int width;
		int height;
		std::vector<glm::vec2> textureCoords;
	};

	class SpriteSheetScene : public IceEngine::Scene
	{
	public:
		SpriteSheetScene();
		~SpriteSheetScene();
		void Update(float deltaTime) override;
		void Render() override;
		Tile_Info *GetTileByName(const std::string &name);
	private:
		
		IceEngine::Shader *m_shader;
		IceEngine::OrthographicCameraComponent *m_cameraComponent;
		IceEngine::TransformComponent *m_transformComponent;

		uint32_t m_tilesetTexture;
	
		std::vector<glm::vec2> m_textureCoords;
		glm::vec2 m_prevMousePos;

		std::vector<Tile_Info> m_tilesInfo;
		int m_spriteIndex = 0;



		


		IceEngine::Tileset *m_tileset;
		IceEngine::Tilemap *m_tilemap;


		
	};
}