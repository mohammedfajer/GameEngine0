#pragma once

#include <glm/glm.hpp>
#include <random>

namespace IceEngine 
{
	glm::vec2 GetRandomPosition(float minX, float maxX, float minY,
		float maxY, float minDistance, const std::vector<glm::vec2>& existingPositions)
	{
		// Seed the random number generator with the current time
		std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

		// Define distributions for x and y within the specified ranges
		std::uniform_real_distribution<float> distX(minX, maxX);
		std::uniform_real_distribution<float> distY(minY, maxY);

		glm::vec2 randomPosition;
		bool tooClose = false;
		do {
			// Generate random x and y values
			randomPosition.x = distX(rng);
			randomPosition.y = distY(rng);

			// Check the distance to existing positions
			tooClose = false;
			for (const auto& existingPos : existingPositions) 
			{
				if (glm::length(randomPosition - existingPos) < minDistance) 
				{
					tooClose = true;
					break;
				}
			}

			// If too close, generate a new position
		} while (tooClose);

		return randomPosition;
	}


	class Random
	{
	public :
		static void Init()
		{
			s_RandomEngine.seed(std::random_device()());
		}

		// samples from random distribution and returns a number between 0 and 1
		static float Float()
		{
			return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}
	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};

}