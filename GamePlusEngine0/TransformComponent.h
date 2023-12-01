#pragma once

#include "Component.h"
#include <glm/glm.hpp>

namespace IceEngine
{
	struct TransformComponent : public Component
	{
		glm::vec2 position;
		glm::vec2 scale;
		float rotation;

		TransformComponent() : position(0.0f), scale(1.0f), rotation(0.0f) {}
		
		void Update(float deltaTime) override
		{
		}
	};
}

