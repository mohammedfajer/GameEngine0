#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

namespace IceEngine {

	struct Sprite {
		glm::vec2 position;
		glm::vec2 scale;

		glm::mat4 model;
		float rotation;

		uint32_t texture;
		std::string name;

		Sprite(const glm::vec2& position, const glm::vec2& scale, uint32_t texture, std::string name = "untitled")
			: position(position), scale(scale), texture(texture), rotation(0.0f), name(name) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(position, 0.0f));
			model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(scale, 1.0f));
		}
	};


	bool areSpritesTooClose(const Sprite& sprite1, const Sprite& sprite2, float minDistance);
	glm::vec2 generateRandomPosition(const std::vector<Sprite>& sprites, float minX, float maxX,
		float minY, float maxY, float minDistance);


}