
#include "Sprite.h"

#include <random>

namespace IceEngine {


	// Function to check if two sprites are too close to each other
	bool areSpritesTooClose(const Sprite& sprite1, const Sprite& sprite2, float minDistance) {
		float distance = glm::length(sprite1.position - sprite2.position);
		return distance < minDistance;
	}

	// Function to generate random non-overlapping positions
	glm::vec2 generateRandomPosition(const std::vector<Sprite>& sprites, float minX, float maxX,
		float minY, float maxY, float minDistance) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> disX(minX, maxX);
		std::uniform_real_distribution<float> disY(minY, maxY);

		glm::vec2 randomPosition;
		bool tooClose;

		do {
			tooClose = false;
			randomPosition.x = disX(gen);
			randomPosition.y = disY(gen);

			for (const auto& sprite : sprites) {
				if (areSpritesTooClose(sprite, { randomPosition, glm::vec2(0.0f, 0.0f), 0 }, minDistance)) {
					tooClose = true;
					break;
				}
			}
		} while (tooClose);

		return randomPosition;
	}



}