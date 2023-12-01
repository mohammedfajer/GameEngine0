#pragma once

#include <glm/glm.hpp>
#include <random>

namespace IceEngine {

	glm::vec2 GetRandomPosition(float minX, float maxX, float minY, float maxY, float minDistance) 
	{
		// Seed the random number generator with the current time
		std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

		// Define distributions for x and y within the specified ranges
		std::uniform_real_distribution<float> distX(minX, maxX);
		std::uniform_real_distribution<float> distY(minY, maxY);

		// Generate random x and y values
		float randomX = distX(rng);
		float randomY = distY(rng);

		// Ensure the generated position is at least minDistance away from the origin (0, 0)
		while (glm::length(glm::vec2(randomX, randomY)) < minDistance) 
		{
			randomX = distX(rng);
			randomY = distY(rng);
		}

		return glm::vec2(randomX, randomY);
	}
}