#pragma once
#include "Component.h"
#include <string>
#include "Texture2D.h"
#include "TextureLoader.h"

namespace IceEngine {
	struct SpriteRendererComponent : public Component {
		std::string m_spritePath;
		Texture2D m_texture;

		SpriteRendererComponent(const std::string& path) : m_spritePath(path) {
			m_texture = TextureLoader::LoadTexture(path);
		}

		void Update(float deltaTime) override {
			// Add any sprite rendering logic here
		}
	};
}

