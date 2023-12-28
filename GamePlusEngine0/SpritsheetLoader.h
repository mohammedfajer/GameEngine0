#pragma once

#include "Texture2D.h"

#include <glm/glm.hpp>
#include <vector>

namespace IceEngine
{
	
	static std::vector<glm::vec2> GetTextureCoords(Texture2D imageInfo, int tileY, int tileX, int tileWidth, int tileHeight)
	{
		glm::vec2 topLeft  = { static_cast<float>(tileX) / imageInfo.width, static_cast<float>(tileY) / imageInfo.height };
		glm::vec2 topRight = { static_cast<float>(tileX) / imageInfo.width + static_cast<float>(tileWidth) / imageInfo.width,  static_cast<float>(tileY) / imageInfo.height };

		glm::vec2 bottomRight = {
			static_cast<float>(tileX) / imageInfo.width + static_cast<float>(tileWidth) / imageInfo.width,
			static_cast<float>(tileY) / imageInfo.height + static_cast<float>(tileHeight) / imageInfo.height
		};

		glm::vec2 bottomLeft = {
			static_cast<float>(tileX) / imageInfo.width,
			static_cast<float>(tileY) / imageInfo.height + static_cast<float>(tileHeight) / imageInfo.height
		};

		std::vector<glm::vec2> Result = {
			topLeft,
			topRight,
			bottomRight,
			bottomLeft
		};

		return (Result);
	}

	/*
		API usage

		Texture2D imageTexture = ...
		int tileX = ...
		int tileY = ...
		int tileW = ...
		int tileH = ...

		auto textureCoords = GetTextureCoords(imageTexture, tileX, tileY, tileW, tileH);

		Renderer::DrawQuad(...., textureCoords);
	*/



}